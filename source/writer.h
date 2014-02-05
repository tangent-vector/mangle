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
#ifndef MANGLE_WRITER_H_INCLUDED
#define MANGLE_WRITER_H_INCLUDED

/*
Interface for writing characters. Currently supports writing to memory
buffers, along with a "writer" that simply counts characters. Other writers
would be easy to define (just requires a `MgPutChar` callback).
*/

#include "string.h"

typedef struct MgWriterT MgWriter;

typedef void (*MgPutCharFunc)( MgWriter*, int );

struct MgWriterT
{
    MgPutCharFunc   putCharFunc;
    void*           userData;
};

/*
Set up a writer that calls the `putCharFunc` callback for each output
character, passing the specified `userData` as an extra argument.
*/
void MgInitializeWriter(
    MgWriter*       writer,
    MgPutCharFunc   putCharFunc,
    void*           userData );

/*
Write a single character to a writer.

Note: this function takes an `int` paramter, but only `char` values
should be passed in (all other values will be cast to `char`).
*/
void MgPutChar(
    MgWriter*   writer,
    int         value );

/*
Write all characters in range starting at pointer `begin` up to
the pointer `end` (not inclusive).
*/
void MgWriteRange(
    MgWriter* writer,
    char const* begin,
    char const* end );

/*
Write all characters in `string`.
*/
void MgWriteString(
    MgWriter* writer,
    MgString  string );

/*
Write the null-terminated string `string`.
*/
void MgWriteCString(
    MgWriter*   writer,
    char const* string);

/*
Initialize a writer that will simply count the number of character written.
Once you've finished writing your data, the provided `counter` will contain
the count.
*/
void MgInitializeCountingWriter(
    MgWriter*   writer,
    int*        counter );

/*
Initialize a writer that will output data to the specified `buffer`.
It is the responsibility of the user to allocate the right amount of
data (that is, you should probably have used a "counting" writer first).
*/
void MgInitializeMemoryWriter(
    MgWriter*   writer,
    void*       buffer );

#endif /* MANGLE_WRITER_H_INCLUDED */
