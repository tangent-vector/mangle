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
#include "writer.h"

#include <stdlib.h>
#include <string.h> /* strlen */

// MgWriter

void MgInitializeWriter(
    MgWriter*     writer,
    MgPutCharFunc putCharFunc,
    void*       userData )
{
    writer->putCharFunc = putCharFunc;
    writer->userData    = userData;
}

void MgPutChar(
    MgWriter*     writer,
    int         value )
{
    writer->putCharFunc( writer, value );
}

void MgWriteRange(
    MgWriter* writer,
    char const* begin,
    char const* end)
{
    char const* cursor = begin;
    while( cursor != end )
        MgPutChar( writer, *cursor++ );
}

void MgWriteString(
    MgWriter* writer,
    MgString  string )
{
    MgWriteRange( writer, string.begin, string.end );
}

void MgWriteCString(
    MgWriter* writer,
    char const* text)
{
    MgWriteRange(writer, text, text + strlen(text));
}

// MemoryWriter

void MemoryWriter_PutChar(
    MgWriter* writer,
    int     value )
{
    char* cursor = (char*) writer->userData;
    *cursor++ = (char) value;
    writer->userData = cursor;
}

void MgInitializeMemoryWriter(
    MgWriter* writer,
    void*   data )
{
    MgInitializeWriter(
        writer,
        &MemoryWriter_PutChar,
        data );
}

// CountingWriter

void CountingWriter_PutChar(
    MgWriter* writer,
    int     value )
{
    int* counter = (int*) writer->userData;
    ++(*counter);
}

void MgInitializeCountingWriter(
    MgWriter* writer,
    int*    counter )
{
    MgInitializeWriter(
        writer,
        &CountingWriter_PutChar,
        counter );
    *counter = 0;
}

