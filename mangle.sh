#!/bin/bash

# Determine whcih is newer, between mangle.c and .mangle/mangle
if [[ !( mangle.c -nt .mangle/mangle ) ]]; then
	# If the binary is up to date, then just run it and move on
	.mangle/mangle "$@"
	exit
fi

# Otherwise, try to build a binary from mangle.c
: ${CC:="cc"}
mkdir -p .mangle
$CC -o .mangle/mangle -c mangle.c

# And now that it has (hopefully) been built, we run it
.mangle/mangle "$@"
