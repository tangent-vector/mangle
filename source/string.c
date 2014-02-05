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
#include "string.h"

#include "reader.h"

#include <ctype.h>
#include <string.h> /* strlen */

MgString MgMakeString(
    char const* begin,
    char const* end)
{
    MgString result;
    result.begin    = begin;
    result.end      = end;
    return result;
}

MgString MgMakeEmptyString()
{
    return MgMakeString(MG_NULL, MG_NULL);
}

MgString MgTerminatedString(
    char const* begin)
{
    return MgMakeString(begin, begin + strlen(begin));
}

MgBool MgStringsAreEqual(
    MgString left,
    MgString right )
{
    MgReader leftReader;
    MgReader rightReader;

    MgInitializeStringReader( &leftReader, left );
    MgInitializeStringReader( &rightReader, right );

    while( !MgAtEnd(&leftReader)
        && !MgAtEnd(&rightReader) )
    {
        int c = MgGetChar(&leftReader);
        int d = MgGetChar(&rightReader);

        if( c != d )
            return MG_FALSE;
    }

    return MgAtEnd(&leftReader) == MgAtEnd(&rightReader);
}

MgBool MgStringsAreEqualNoCase(
    MgString left,
    MgString right )
{
    char const* leftCursor = left.begin;
    char const* rightCursor = right.begin;

    for(;;)
    {
        MgBool leftAtEnd = leftCursor == left.end;
        MgBool rightAtEnd = rightCursor == right.end;
        if( leftAtEnd || rightAtEnd )
            return leftAtEnd == rightAtEnd;

        char leftChar = *leftCursor++;
        char rightChar = *rightCursor++;
        if( tolower(leftChar) != tolower(rightChar) )
            return MG_FALSE;
    }
}