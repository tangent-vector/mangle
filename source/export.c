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
#include "export.h"

#include <mangle/mangle.h>

#include "reader.h"

MgAttribute* MgFindAttribute(
    MgElement*  pp,
    char const* id )
{
    MgString idString = { id, id + strlen(id) };
    for(MgAttribute* attr = pp->firstAttr; attr; attr = attr->next)
    {
        if( MgStringsAreEqual(attr->id, idString) )
            return attr;
    }
    return 0;
}

MgBool TextIsSameAsFileOnDisk(
    MgString  text,
    char const* filePath)
{
    MgReader reader;
    FILE* file = fopen(filePath, "rb");
    if( !file )
        return MG_FALSE;

    MgInitializeStringReader(&reader, text);

    for(;;)
    {
        int c = fgetc(file);
        int d = MgGetChar(&reader);
        if( c != d )
        {
            return MG_FALSE;
        }

        if( c == -1 )
            return MG_TRUE;
    }
}

void MgWriteTextToFile(
    MgString      text,
    char const* filePath)
{
    if( TextIsSameAsFileOnDisk(text, filePath) )
    {
//        fprintf(stderr, "Skipping export of \"%s\"\n", nameBuffer);
        return;
    }

    FILE* file = fopen(filePath, "wb");
    if( !file )
    {
        fprintf(stderr, "Failed to open \"%s\" for writing\n", filePath);
        return;
    }

    fwrite(text.begin, 1, text.end - text.begin, file);

    fclose(file);
}
