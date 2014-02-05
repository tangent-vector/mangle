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

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "export.h"
#include "string.h"
#include "reader.h"
#include "writer.h"

//

void ExportScrapFileGroup(
    MgContext*        context,
    MgScrapFileGroup* scrapFileGroup,
    MgWriter*         writer );

void ExportScrapElements(
    MgContext*  context,
    MgScrap*      scrap,
    MgElement*  firstElement,
    MgWriter*     writer );

void WriteInt(
    MgWriter*     writer,
    int         value)
{
    enum
    {
        kBufferSize = 16,
    };
    char buffer[kBufferSize];
    char* cursor = &buffer[kBufferSize];
    *(--cursor) = 0;

    int remaining = value;
    do
    {
        int digit = remaining % 10;
        remaining = remaining / 10;

        *(--cursor) = '0' + digit;
    } while( remaining != 0 );

    MgWriteCString(writer, cursor);
}

void EmitLineDirectiveAndIndent(
    MgWriter*     writer,
    MgInputFile*  inputFile,
    MgSourceLoc   loc)
{
    MgWriteCString(writer, "\n#line ");
    WriteInt(writer, loc.line);
    MgWriteCString(writer, " \"");
    MgWriteCString(writer, inputFile->path);
    MgWriteCString(writer, "\"\n");

    int col = loc.col;
    for( int ii = 1; ii < col; ++ii )
        MgWriteCString(writer, " ");
}

void ExportScrapElement(
    MgContext*  context,
    MgScrap*      scrap,
    MgElement*  element,
    MgWriter*     writer )
{
    switch( element->kind )
    {
    case kMgElementKind_CodeBlock:
    case kMgElementKind_Text:
        MgWriteString(writer, element->text);
        ExportScrapElements(context, scrap, element->firstChild, writer);
        break;
    case kMgElementKind_LessThanEntity:
        MgWriteCString(writer, "<");
        break;
    case kMgElementKind_GreaterThanEntity:
        MgWriteCString(writer, ">");
        break;
    case kMgElementKind_AmpersandEntity:
        MgWriteCString(writer, "&");
        break;

    case kMgElementKind_ScrapRef:
        {
            MgScrapFileGroup* scrapGroup = MgFindAttribute(element, "$scrap-group")->scrapFileGroup;
            ExportScrapFileGroup(context, scrapGroup, writer);
            MgSourceLoc resumeLoc = MgFindAttribute(element, "$resume-at")->sourceLoc;
            EmitLineDirectiveAndIndent(writer, scrap->fileGroup->inputFile, resumeLoc);
        }
        break;

    default:
        assert(MG_FALSE);
        break;
    }
}

void ExportScrapElements(
    MgContext*  context,
    MgScrap*      scrap,
    MgElement*  firstElement,
    MgWriter*     writer )
{
    for( MgElement* element = firstElement; element; element = element->next )
        ExportScrapElement( context, scrap, element, writer );
}

void ExportScrapText(
    MgContext*    context,
    MgScrap*      scrap,
    MgWriter*     writer )
{
    EmitLineDirectiveAndIndent(writer, scrap->fileGroup->inputFile, scrap->sourceLoc);
    ExportScrapElements(
        context,
        scrap,
        scrap->body,
        writer );
}


void ExportScrapFileGroupImpl(
    MgContext*        context,
    MgScrapFileGroup* fileGroup,
    MgWriter*         writer )
{
    MgScrap* scrap = fileGroup->firstScrap;
    while( scrap )
    {
        ExportScrapText( context, scrap, writer );
        scrap = scrap->next;
    }
}

void ExportScrapNameGroupImpl(
    MgContext*        context,
    MgScrapNameGroup* nameGroup,
    MgWriter*         writer )
{
    MgScrapFileGroup* fileGroup = nameGroup->firstFileGroup;
    while( fileGroup )
    {
        ExportScrapFileGroupImpl( context, fileGroup, writer );
        fileGroup = fileGroup->next;
    }
}


void ExportScrapFileGroup(
    MgContext*        context,
    MgScrapFileGroup* fileGroup,
    MgWriter*         writer )
{
    switch( fileGroup->nameGroup->kind )
    {
    default:
        assert(0);
        break;

    case kScrapKind_GlobalMacro:
    case kScrapKind_OutputFile:
        ExportScrapNameGroupImpl(context, fileGroup->nameGroup, writer);
        break;

    case kScrapKind_Unknown:
    case kScrapKind_LocalMacro:
        ExportScrapFileGroupImpl(context, fileGroup, writer);
        break;
    }
}

void MgWriteCodeFile(
    MgContext*          context,
    MgScrapNameGroup*   codeFile )
{
    MgString id = codeFile->id;

    // \todo: alloca or malloc the name buffer when required...
    char nameBuffer[1024];
    char* writeCursor = &nameBuffer[0];
    char const* readCursor = id.begin;
    char const* end = id.end;
    while( readCursor != end )
    {
        *writeCursor++ = *readCursor++;
    }
    *writeCursor++ = 0;

    MgWriter writer;

    // now construct the outputString
    int counter = 0;
    MgInitializeCountingWriter( &writer, &counter );
    ExportScrapNameGroupImpl( context, codeFile, &writer );

    int outputSize = counter;
    char* outputBuffer = (char*) malloc(outputSize + 1);
    outputBuffer[outputSize] = 0;
    
    MgInitializeMemoryWriter( &writer, outputBuffer );
    ExportScrapNameGroupImpl( context, codeFile, &writer );

    MgString outputText = MgMakeString( outputBuffer, outputBuffer + outputSize );
    MgWriteTextToFile(outputText, nameBuffer);
}

MgScrapNameGroup* MgGetFirstCodeFile(
    MgContext* context)
{
    MgScrapNameGroup* group = context->firstScrapNameGroup;
    while( group && group->kind != kScrapKind_OutputFile )
        group = group->next;
    return group;
}

MgScrapNameGroup* MgGetNextCodeFile(
    MgScrapNameGroup* codeFile)
{
    MgScrapNameGroup* group = codeFile ? codeFile->next : MG_NULL;
    while( group && group->kind != kScrapKind_OutputFile )
        group = group->next;
    return group;
}

void MgWriteAllCodeFiles(
    MgContext* context)
{
    MgScrapNameGroup* codeFile = MgGetFirstCodeFile( context );
    while( codeFile )
    {
        MgWriteCodeFile( context, codeFile );
        codeFile = MgGetNextCodeFile( codeFile );
    }
}
