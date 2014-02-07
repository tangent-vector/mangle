#!/bin/bash

# This is a slightly kludgy script for generating a "self-building" shell
# script. We put a uuencoded tar archive of files after a marker (`ARCHIVE:`)
# in the generated script, and then the script searches itself for this
# marker so that it can extract the archive into a directory before `cd`ing
# into it and running `make`.
#
# We also include some logic at the start of the script to see if there
# already exists a binary, and if so we run that binary directly rather
# than making it first.
#
# For most purposes, a users should be able to use the shell script as if
# if were the actual build tool, and ignore the intermediate cruft that
# happens on a first use.
#
# TODO: make a version of this that plays nice with Windows.

if [[ ! "$1" ]]; then
	echo "usage: $0 <name> <file>..."
	exit 1
fi

name=$1

echo "#!/bin/bash"                                                             > "${name}.sh"
# fast path: check if binary already built, and appears up-to-date
echo "if [[ !( \"\$0\" -nt \".${name}/${name}\" ) ]]; then"                   >> "${name}.sh"
echo "	\".${name}/${name}\" \"\$@\""                                         >> "${name}.sh"
echo "	exit"                                                                 >> "${name}.sh"
echo "fi"                                                                     >> "${name}.sh"
# otherwise we need to try and extract the archive and build it
echo "mkdir -p \".${name}\""                                                  >> "${name}.sh"
echo "match=\$(grep --text --line-number '^ARCHIVE:$' \$0 | cut -d ':' -f 1)" >> "${name}.sh"
echo "archive_start=\$((match + 1))"                                          >> "${name}.sh"
echo "tail -n +\$archive_start \$0 | uudecode | tar -xzf - -C \".${name}\""   >> "${name}.sh"
echo "(cd \".${name}\"; rm \"${name}\"; make -s \"${name}\")"                 >> "${name}.sh"
echo "\".${name}/${name}\" \"\$@\""                                           >> "${name}.sh"
echo "exit"                                                                   >> "${name}.sh"
# the archive comes at the end of the file after a marker
echo "ARCHIVE:"                                                               >> "${name}.sh"
tar -cz "${@:2}" | uuencode - >> "${name}.sh"
chmod u+x "${name}.sh"
