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

#include "export.h"
#include "reader.h"
#include "writer.h"

#include <assert.h> /* assert */
#include <string.h> /* strlen */
#include <stdlib.h> /* malloc */

void WriteElements(
    MgContext*  context,
    MgElement*  firstElement,
    MgWriter*     writer );

void WriteAttributes(
    MgElement* pp,
    MgWriter*     output )
{
    for(MgAttribute* attr = pp->firstAttr; attr; attr = attr->next)
    {
        if( attr->id.begin[0] == '$' )
            continue;

        MgWriteCString(output, " ");
        MgWriteString( output, attr->id );
        MgWriteCString(output, "=\"");
        MgWriteString( output, attr->val );
        MgWriteCString(output, "\"");
    }
}

void WriteScrapGroupName(
    MgContext*        context,
    MgWriter*         writer,
    MgScrapNameGroup* scrapGroup)
{
    if( scrapGroup->name )
    {
        WriteElements(context, scrapGroup->name, writer);
    }
    else
    {
        MgWriteString(writer, scrapGroup->id);
    }

}

void WriteElement(
    MgContext*    context,
    MgElement*  pp,
    MgWriter*     output )
{
    MgElementKind kind = pp->kind;
    switch( kind )
    {
    default:
        fprintf(stderr, "unknown: %d\n", kind);
        assert(0);
        break;

    case kMgElementKind_Table:
        MgWriteCString(output, "<table>");
        break;
    case kMgElementKind_TableRow:
        MgWriteCString(output, "<tr>");
        break;
    case kMgElementKind_TableHeader:
        MgWriteCString(output, "<th>");
        break;
    case kMgElementKind_TableCell:
        MgWriteCString(output, "<td>");
        break;

    case kMgElementKind_LessThanEntity:
        MgWriteCString(output, "&lt;");
        break;
    case kMgElementKind_GreaterThanEntity:
        MgWriteCString(output, "&gt;");
        break;
    case kMgElementKind_AmpersandEntity:
        MgWriteCString(output, "&amp;");
        break;

    case kMgElementKind_Text:
    case kMgElementKind_HtmlBlock:
        break;

    case kMgElementKind_Link:
        {
            MgWriteCString(output, "<a");
            WriteAttributes(pp, output);
            MgWriteCString(output, ">");
        }
        break;
    case kMgElementKind_ReferenceLink:
        {
            MgReferenceLink* ref = MgFindAttribute(pp, "$referenceLink")->referenceLink;
            MgWriteCString(output, "<a href=\"");
            MgWriteString(output, ref->url);
            MgWriteCString(output, "\">");
        }
        break;
    case kMgElementKind_ScrapDef:
        {
            MgScrap* scrap = MgFindAttribute(pp, "$scrap")->scrap;
            MgWriteCString(output, "<div class='scrap-def'>&#x3008;<span class='scrap-name'>");
            // output the scrap name (\todo: properly formatted)
            WriteScrapGroupName(context, output, scrap->fileGroup->nameGroup);
            MgWriteCString(output, "</span>&#x3009;");
            // TODO: fix this logic for local macros...
            if( scrap->fileGroup != scrap->fileGroup->nameGroup->firstFileGroup
                || scrap != scrap->fileGroup->firstScrap )
                MgWriteCString(output, "+");
            MgWriteCString(output, "&#x2261;<br />\n");
        }
        break;
    case kMgElementKind_ScrapRef:
        {
            MgScrapFileGroup* scrapGroup = MgFindAttribute(pp, "$scrap-group")->scrapFileGroup;
            MgWriteCString(output, "<span class='scrap-ref'>&#x3008;<span class='scrap-name'>");
            // output the scrap name (\todo: properly formatted)
            WriteScrapGroupName(context, output, scrapGroup->nameGroup);
            MgWriteCString(output, "</span>&#x3009;");
        }
        break;
    case kMgElementKind_Em:
        MgWriteCString(output,
            "<em>");
        break;
    case kMgElementKind_Strong:
        MgWriteCString(output,
            "<strong>");
        break;
    case kMgElementKind_Paragraph:
        MgWriteCString(output,
            "<p>");
        break;
    case kMgElementKind_BlockQuote:
        MgWriteCString(output,
            "<blockquote>");
        break;
    case kMgElementKind_CodeBlock:
        MgWriteCString(output,
            "<pre><code>");
        break;
    case kMgElementKind_InlineCode:
        MgWriteCString(output,
            "<code>");
        break;
    case kMgElementKind_HorizontalRule:
        MgWriteCString(output,
            "<hr>");
        break;
    case kMgElementKind_UnorderedList:
        MgWriteCString(output,
            "<ul>");
        break;
    case kMgElementKind_OrderedList:
        MgWriteCString(output,
            "<ol>");
        break;
    case kMgElementKind_ListItem:
        MgWriteCString(output,
            "<li>");
        break;
    case kMgElementKind_Header1:
    case kMgElementKind_Header2:
    case kMgElementKind_Header3:
    case kMgElementKind_Header4:
    case kMgElementKind_Header5:
    case kMgElementKind_Header6:
        MgWriteCString(output, "<h");
        MgPutChar(output, '1' + (kind - kMgElementKind_Header1));
        MgWriteCString(output, ">");
        break;
    }

    MgWriteString(output, pp->text);
    WriteElements(context, pp->firstChild, output);

    switch( kind )
    {
    default:
        assert(0);
        break;

    case kMgElementKind_Table:
        MgWriteCString(output, "</table>");
        break;
    case kMgElementKind_TableRow:
        MgWriteCString(output, "</tr>");
        break;
    case kMgElementKind_TableHeader:
        MgWriteCString(output, "</th>");
        break;
    case kMgElementKind_TableCell:
        MgWriteCString(output, "</td>");
        break;

    case kMgElementKind_LessThanEntity:
    case kMgElementKind_GreaterThanEntity:
    case kMgElementKind_AmpersandEntity:
        break;

    case kMgElementKind_Text:
    case kMgElementKind_HtmlBlock:
        break;
    case kMgElementKind_Link:
    case kMgElementKind_ReferenceLink:
        MgWriteCString(output,
            "</a>");
        break;
    case kMgElementKind_ScrapDef:
        MgWriteCString(output, "</div>");
        break;
    case kMgElementKind_ScrapRef:
        MgWriteCString(output, "</span>");
        break;
    case kMgElementKind_Em:
        MgWriteCString(output,
            "</em>");
        break;
    case kMgElementKind_Strong:
        MgWriteCString(output,
            "</strong>");
        break;
    case kMgElementKind_Paragraph:
        MgWriteCString(output,
            "</p>\n");
        break;
    case kMgElementKind_BlockQuote:
        MgWriteCString(output,
            "</blockquote>");
        break;
    case kMgElementKind_CodeBlock:
        MgWriteCString(output,
            "</code></pre>");
        break;
    case kMgElementKind_InlineCode:
        MgWriteCString(output,
            "</code>");
        break;
    case kMgElementKind_HorizontalRule:
        break;
    case kMgElementKind_UnorderedList:
        MgWriteCString(output,
            "</ul>");
        break;
    case kMgElementKind_OrderedList:
        MgWriteCString(output,
            "</ol>");
        break;
    case kMgElementKind_ListItem:
        MgWriteCString(output,
            "</li>");
        break;
    case kMgElementKind_Header1:
    case kMgElementKind_Header2:
    case kMgElementKind_Header3:
    case kMgElementKind_Header4:
    case kMgElementKind_Header5:
    case kMgElementKind_Header6:
        MgWriteCString(output, "</h");
        MgPutChar(output, '1' + (kind - kMgElementKind_Header1));
        MgWriteCString(output, ">");
        break;
    }

}

void WriteElements(
    MgContext*  context,
    MgElement*  firstElement,
    MgWriter*     writer )
{
    for( MgElement* element = firstElement; element; element = element->next )
    {
        WriteElement( context, element, writer );
    }
}

//

static void WriteToFile(
    FILE* file,
    char const* begin,
    char const* end)
{
    fwrite(begin, 1, end-begin, file);
}

void MgWriteDoc(
    MgContext*        context,
    MgInputFile*      inputFile,
    MgWriter*         writer )
{
    MgWriteCString(writer,
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
        "    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
        "<head>\n"
        "    <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n"
        "    <title>Page Title</title>\n"
//*
        "<script src='http://use.edgefonts.net/open-sans:n3,i3,n4,i4,n6,i6,n7,i7,n8,i8.js'></script>"
        "<script src='http://use.edgefonts.net/open-sans-condensed:n3,i3,n7.js'></script>"
        "<script src='http://use.edgefonts.net/average:n4.js'></script>"
        "<script src='http://use.edgefonts.net/source-code-pro.js'></script>"
        "<link rel='stylesheet' type='text/css' href='test.css'>"
//*/
        "</head>\n"
        "<body>\n");

    WriteElements(
        context,
        inputFile->firstElement,
        writer );

    MgWriteCString(writer,
        "</body>\n");
}

char* stpcpy(char* dest, char const* src)
{
    while( *dest++ = *src++ )
    {}
    return dest-1;
}

void MgWriteDocFileToPath(
    MgContext*    context,
    MgInputFile*  inputFile,
    const char* path)
{
    MgWriter writer;

    int counter = 0;
    MgInitializeCountingWriter( &writer, &counter );

    // run the export logic once, to count the size needed
    MgWriteDoc( context, inputFile, &writer );

    int size = counter;
    char* data = (char*) malloc(size + 1);
    data[size] = 0; // \todo: shouldn't be required

    MgInitializeMemoryWriter( &writer, data );

    // run the export logic again, this time writing to the output buffer
    MgWriteDoc( context, inputFile, &writer );

    MgString outputText = MgMakeString( data, data + size );

    MgWriteTextToFile(outputText, path);
}

void MgWriteDocFile(
    MgContext* context,
    MgInputFile* inputFile)
{
    // compute path for output file...

    // find just the name part of the input file path
    char const* inputFilePath = inputFile->path;
    char const* docOutputPath = ""; // \todo: options parsing!!!
    char const* slash = strrchr(inputFilePath, '/');
    char const* inputFileName = slash ? slash+1 : inputFilePath;

    // allocate six extra characters for worst-case ".html\0"
    int outputPathSize = strlen(docOutputPath) + strlen(inputFileName) + 6;
    char* outputFileName = (char*) malloc(outputPathSize);
    char* cursor = outputFileName;
    cursor = stpcpy(cursor, docOutputPath);
    cursor = stpcpy(cursor, inputFileName);
    char* dot = strrchr(outputFileName, '.');
    cursor = strcpy(dot ? dot : cursor, ".html");

    MgWriteDocFileToPath(
        context,
        inputFile,
        outputFileName );

    free(outputFileName);
}

MgInputFile* MgGetFirstInputFile(
    MgContext* context)
{
    if( !context ) return 0;
    return context->firstInputFile;
}

MgInputFile* MgGetNextInputFile(
    MgInputFile*  inputFile)
{
    if( !inputFile ) return 0;
    return inputFile->next;
}

void MgWriteAllDocFiles(
    MgContext* context )
{
    MgInputFile* inputFile = MgGetFirstInputFile( context );
    while( inputFile )
    {
        MgWriteDocFile( context, inputFile );
        inputFile = MgGetNextInputFile( inputFile );
    }
}
