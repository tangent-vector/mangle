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
#include <mangle/mangle.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "export.h"
#include "parse.h"
#include "reader.h"
#include "string.h"
#include "writer.h"

//

MgString ReadLineText(
    MgReader* reader )
{
    char const* textBegin = reader->cursor;
    char const* textEnd = textBegin;

    for(;;)
    {
        int c, d;
        c = MgGetChar(reader);
        switch( c )
        {
        default:
            textEnd = reader->cursor;
            continue;

        case '\r':
        case '\n':
            d = MgGetChar(reader);
            if( (c ^ d) != ('\r' ^ '\n') )
            {
                MgUnGetChar(reader, d);
            }

            // fall-through:
        case -1:
            // either a newline or the end of the file
            return MgMakeString(textBegin, textEnd);
        }
    }    
}

void ReadLine(
    MgReader*   reader,
    MgLine*         line )
{
    MgString text = ReadLineText( reader );
    line->originalBegin = text.begin;
    line->text.begin = text.begin;
    line->text.end   = text.end;
}

void ReadLines(
    MgReader*   reader,
    MgLine*         beginLines,
    MgLine*         endLines )
{
    MgLine* lineCursor = beginLines;

    // always at least one line
    ReadLine( reader, lineCursor++ );
    while(!MgAtEnd(reader))
    {
        ReadLine( reader, lineCursor++ );
    }
}

int MgCountLinesInString(
    MgString string )
{
    MgReader reader;
    MgInitializeStringReader( &reader, string );

    // always at least one line
    MgString lineText = ReadLineText( &reader );
    int lineCount = 1;
    while(!MgAtEnd(&reader))
    {
        lineText = ReadLineText( &reader );
        ++lineCount;
    }
    return lineCount;
}

void MgReadLinesFromString(
    MgString  string,
    MgLine* beginLines,
    MgLine* endLines )
{
    MgReader reader;
    MgInitializeStringReader( &reader, string );
    ReadLines( &reader, beginLines, endLines );
}

//

void MgParseInputFileText(
    MgContext*    context,
    MgInputFile*  inputFile)
{
    int lineCount = MgCountLinesInString( inputFile->text );
    MgLine* beginLines = (MgLine*) malloc(lineCount * sizeof(MgLine));
    MgLine* endLines = beginLines + lineCount;
    inputFile->beginLines = beginLines;
    inputFile->endLines = endLines;

    MgReadLinesFromString(
        inputFile->text,
        beginLines,
        endLines );

    MgElement* firstElement = MgParseBlockElements(
        context,
        inputFile,
        beginLines,
        endLines );
    inputFile->firstElement = firstElement;
}

void MgInitialize(
    MgContext*  context )
{
    memset(context, 0, sizeof(*context));
}

void MgFinalizeElements(
    MgContext*      context,
    MgElement*      firstElement );

void MgFinalizeElement(
    MgContext*      context,
    MgElement*      element )
{
    MgFinalizeElements( context, element->firstChild );

    MgAttribute* attr = element->firstAttr;
    while( attr )
    {
        MgAttribute* next = attr->next;
        free( attr );
        attr = next;
    }
}

void MgFinalizeElements(
    MgContext*      context,
    MgElement*      firstElement )
{
    MgElement* element = firstElement;
    while( element )
    {
        MgElement* next = element->next;
        MgFinalizeElement( context, element );
        free( element );
        element = next;
    }
}

void MgFinalizeInputFile(
    MgContext*      context,
    MgInputFile*    file)
{
    if( file->allocatedFileData )
        free( file->allocatedFileData );
    if( file->beginLines )
        free( file->beginLines );

    MgFinalizeElements( context, file->firstElement );

    MgReferenceLink* link = file->firstReferenceLink;
    while( link )
    {
        MgReferenceLink* next = link->next;
        free(link);
        link = next;
    }
}

void MgFinalizeScrapFileGroup(
    MgContext*          context,
    MgScrapFileGroup*   group)
{
    MgScrap* scrap = group->firstScrap;
    while( scrap )
    {
        MgScrap* next = scrap->next;
        // NOTE: the `body` of a scrap is already
        // stored elsewhere in the document structure,
        // so we don't free it here...
        free( scrap );
        scrap = next;
    }
}

void MgFinalizeScrapNameGroup(
    MgContext*          context,
    MgScrapNameGroup*   group)
{
    MgScrapFileGroup* subGroup = group->firstFileGroup;
    while( subGroup )
    {
        MgScrapFileGroup* next = subGroup->next;
        MgFinalizeScrapFileGroup( context, subGroup );
        free( subGroup );
        subGroup = next;
    }
}

void MgFinalize(
    MgContext*  context)
{
    MgInputFile* file = context->firstInputFile;
    while( file )
    {
        MgInputFile* next = file->next;
        MgFinalizeInputFile( context, file );
        free( file );
        file = next;
    }

    MgScrapNameGroup* group = context->firstScrapNameGroup;
    while( group )
    {
        MgScrapNameGroup* next = group->next;
        MgFinalizeScrapNameGroup( context, group );
        free( group );
        group = next;
    }
}

MgInputFile* MgAddInputFileText(
    MgContext*    context,
    const char* path,
    const char* textBegin,
    const char* textEnd )
{
    if( !context )      return 0;
    if( !path )         return 0;
    if( !textBegin )    return 0;

    if( !textEnd )
    {
        textEnd = textBegin + strlen(textBegin);
    }

    MgString text = { textBegin, textEnd };

    MgInputFile* inputFile = (MgInputFile*) malloc(sizeof(MgInputFile));
    if( !inputFile )
        return 0;
    inputFile->path         = path;
    inputFile->text         = text;
    inputFile->firstElement = 0;
    inputFile->next         = 0;
    inputFile->allocatedFileData = 0;
    inputFile->firstReferenceLink = 0;

    if( context->lastInputFile )
    {
        context->lastInputFile->next = inputFile;
    }
    else
    {
        context->firstInputFile = inputFile;
    }
    context->lastInputFile = inputFile;

    MgParseInputFileText(
        context,
        inputFile );

    return inputFile;
}

MgInputFile* MgAddInputFileStream(
    MgContext*  context,
    char const* path,
    FILE*       stream )
{
    if( !context )  return 0;
    if( !stream )   return 0;

    int begin = ftell(stream);
    fseek(stream, 0, SEEK_END);
    int end = ftell(stream);
    fseek(stream, begin, SEEK_SET);
    int size = end - begin;

    // allocate buffer for input file
    char* fileData = (char*) malloc(size + 1);
    // we NULL-terminate the buffer just in case
    // (but the code should never rely on this)
    fileData[size] = 0;

    int sizeRead = fread(fileData, 1, size, stream);
    if( sizeRead != size )
    {
        fprintf(stderr, "failed to read from \"%s\"\n", path);
        return 0;
    }

    MgString text = { fileData, fileData + size };
    MgInputFile* inputFile = MgAddInputFileText(
        context,
        path,
        fileData,
        fileData + size );
    inputFile->allocatedFileData = fileData;
    return inputFile;
}

MgInputFile* MgAddInputFilePath(
    MgContext*    context,
    const char* path )
{
    FILE* stream = MG_NULL;
    if( !context )  return 0;
    if( !path )     return 0;

    stream = fopen(path, "rb");
    if( !stream )
    {
        fprintf(stderr, "mangle: failed to open \"%s\" for reading\n", path);
        return 0;
    }

    MgInputFile* inputFile = MgAddInputFileStream(
        context,
        path,
        stream );
    fclose(stream);
    return inputFile;
}
