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
#ifndef MANGLE_READER_H_INCLUDED
#define MANGLE_READER_H_INCLUDED

/*
A simple interface for reading from an `MgString`.
*/

#include "string.h"

typedef struct MgReaderT
{
    MgString    string;
    char const* cursor;
} MgReader;

/*
The `MgGetChar` and `MgPeekChar` functions will return `kMgEndOfFile` when
trying to read past the end of the string.
*/
enum
{
    kMgEndOfFile = -1,
};

/*
Set up to start reading from `string`.
*/
void MgInitializeStringReader(
    MgReader* reader,
    MgString          string );

/*
Return `MG_TRUE` if reader is at end.
*/
MgBool MgAtEnd(
    MgReader* reader );

/*
Get next character from the reader, or `kMgEndOfFile` if at end.
*/
int MgGetChar(
    MgReader* reader );

/*
Back up the reader by one character. The `value` passed in should match
the most recent character (or end-of-file marker) read from the reader.
*/
void MgUnGetChar(
    MgReader* reader,
    int             value );

/*
Get the character before the current position in the reader, or
`kMgEndOfFile` if we are at the beginning of the string.

TODO: Is this function really needed? It is a bit gross to have.
*/
int MgGetPrecedingChar(
    MgReader* reader );

/*
Return the next character that `MgGetChar` would yield, without advancing
the cursor of the reader.
*/
int MgPeekChar(
    MgReader* reader );

#endif /* MANGLE_READER_H_INCLUDED */
