#!/bin/bash

pushd `dirname $0` > /dev/null
MANGLEPATH=`pwd`
popd > /dev/null


# Determine whcih is newer, between mangle.c and .mangle/mangle
if [[ !( "$MANGLEPATH/mangle.c" -nt "$MANGLEPATH/mangle" ) ]]; then
	# If the binary is up to date, then just run it and move on
	"$MANGLEPATH/mangle" "$@"
	exit
fi

# Otherwise, try to build a binary from mangle.c
: ${CC:="cc"}

pushd "$MANGLEPATH" > /dev/null
$CC mangle.c -o mangle
popd > /dev/null

# And now that it has (hopefully) been built, we run it
"$MANGLEPATH/mangle" "$@"
