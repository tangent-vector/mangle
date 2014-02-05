/****************************************************************************
Copyright (c) 2014 Tim Foley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef MANGLE_STRING_H_INCLUDED
#define MANGLE_STRING_H_INCLUDED

#include <mangle/mangle.h>

#include "common.h"

/*
Make a string representing the characters between the pointers `begin` and
`end` (exclusive).
*/
MgString MgMakeString(
    char const* begin,
    char const* end );

/*
Make an empty string (both pointers NULL).
*/
MgString MgMakeEmptyString();

/*
Make an `MgString` from a null-terminated string. That is, point `end` at
the terminating null byte.
*/
MgString MgTerminatedString(
    char const* begin );

/*
Return true iff the `left` and `right` strings are equal.
*/
MgBool MgStringsAreEqual(
    MgString left,
    MgString right );

/*
Return true iff the `left` and `right` strings are equal, modulo case
(for standard ASCII characters).
*/
MgBool MgStringsAreEqualNoCase(
    MgString left,
    MgString right );

/*
Return true iff the given `string` is empty.
*/
static MgBool MgIsEmptyString(
    MgString string)
{
    return string.begin == string.end;
}

#endif /* MANGLE_STRING_H_INCLUDED */
