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
#include "parse.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "export.h"
#include "reader.h"
#include "string.h"
#include "writer.h"

//

enum
{
    kMaxHeaderLevel = 6,
};


MgReferenceLink* MgFindOrCreateReferenceLink(
    MgInputFile*    inputFile,
    MgString          id )
{
    MgReferenceLink* link = inputFile->firstReferenceLink;
    while( link )
    {
        if( MgStringsAreEqualNoCase(
            id,
            link->id  ) )
        {
            return link;
        }
        link = link->next;
    }

    link = (MgReferenceLink*) malloc(sizeof(MgReferenceLink));
    link->id    = id;
    link->url   = MgMakeEmptyString();
    link->title = MgMakeEmptyString();

    link->next = inputFile->firstReferenceLink;
    inputFile->firstReferenceLink = link;

    return link;
}

MgAttribute* MgAddAttribute(
    MgElement*  element,
    char const* id,
    MgString      val )
{
    MgAttribute* attr = (MgAttribute*) malloc(sizeof(MgAttribute));
    attr->next  = MG_NULL;
    attr->id    = MgTerminatedString(id);
    attr->val   = val;

    // this is a linear-time insert, but we expect the
    // number of attributes per element to be very low,
    // so it probably won't matter for performance.
    MgAttribute** link = &element->firstAttr;
    while( *link )
        link = &((*link)->next);
    *link = attr;
    return attr;
}

MgAttribute* MgAddCustomAttribute(
    MgElement*  element,
    char const* id)
{
    return MgAddAttribute(element, id, MgMakeString(MG_NULL, MG_NULL));
}

MgElement* MgCreateElementImpl(
    MgElementKind   kind,
    MgString          text,
    MgElement*      firstChild )
{
    MgElement* element = (MgElement*) malloc(sizeof(MgElement));
    element->kind       = kind;
    element->text       = text;
    element->firstAttr  = MG_NULL;
    element->firstChild = firstChild;
    element->next       = MG_NULL;
    return element;
}

MgElement* MgCreateLeafElement(
    MgElementKind   kind,
    MgString          text )
{
    return MgCreateElementImpl(
        kind,
        text,
        MG_NULL );  // no children
}

MgElement* MgCreateParentElement(
    MgElementKind   kind,
    MgElement*      firstChild )
{
    return MgCreateElementImpl(
        kind,
        MgMakeString(MG_NULL, MG_NULL), // no text
        firstChild );
}

MgScrapNameGroup* MgFindScrapNameGroup(
    MgContext*    context,
    MgString      id )
{
    MgScrapNameGroup* group = context->firstScrapNameGroup;
    while( group )
    {
        if( MgStringsAreEqual( group->id, id ) )
            return group;

        group = group->next;
    }

    return 0;
}

MgScrapFileGroup* MgFindScrapFileGroup(
    MgScrapNameGroup* nameGroup,
    MgInputFile*      inputFile )
{
    MgScrapFileGroup* fileGroup = nameGroup->firstFileGroup;
    while( fileGroup )
    {
        if( fileGroup->inputFile == inputFile )
            return fileGroup;
    }

    return 0;
}

MgScrapFileGroup* MgFindOrCreateScrapGroup(
    MgContext*    context,
    MgScrapKind   kind,
    MgString      id,
    MgInputFile*  file )
{
    MgScrapNameGroup* nameGroup = MgFindScrapNameGroup( context, id );
    if( !nameGroup )
    {
        nameGroup = (MgScrapNameGroup*) malloc(sizeof(MgScrapNameGroup));
        nameGroup->kind = kind;
        nameGroup->id   = id;
        nameGroup->name = 0;
        nameGroup->firstFileGroup = 0;
        nameGroup->lastFileGroup = 0;
        nameGroup->next = 0;

        if( context->lastScrapNameGroup )
        {
            context->lastScrapNameGroup->next = nameGroup;
        }
        else
        {
            context->firstScrapNameGroup = nameGroup;
        }
        context->lastScrapNameGroup = nameGroup;
    }

    if( nameGroup->kind == kScrapKind_Unknown )
    {
        nameGroup->kind = kind;
    }
    else if( kind != kScrapKind_Unknown
        && kind != nameGroup->kind )
    {
        fprintf(stderr, "incompatible scrap kinds!\n");
    }

    MgScrapFileGroup* fileGroup = MgFindScrapFileGroup( nameGroup, file );
    if( !fileGroup )
    {
        fileGroup = (MgScrapFileGroup*) malloc(sizeof(MgScrapFileGroup));
        fileGroup->nameGroup    = nameGroup;
        fileGroup->inputFile    = file;
        fileGroup->firstScrap   = 0;
        fileGroup->lastScrap    = 0;
        fileGroup->next         = 0;
    
        if( nameGroup->lastFileGroup )
        {
            nameGroup->lastFileGroup->next = fileGroup;
        }
        else
        {
            nameGroup->firstFileGroup = fileGroup;
        }
        nameGroup->lastFileGroup = fileGroup;
    }

    return fileGroup;
}

void MgAddScrapToFileGroup(
    MgScrapFileGroup* fileGroup,
    MgScrap*          scrap)
{
    if( fileGroup->lastScrap )
    {
        fileGroup->lastScrap->next = scrap;
    }
    else
    {
        fileGroup->firstScrap = scrap;
    }
    fileGroup->lastScrap = scrap;
}

MgBool MgCheckMatch(
    MgReader*   reader,
    char            c,
    int             count )
{
    for( int ii = 0; ii < count; ++ii )
    {
        int d = MgGetChar( reader );
        if( d != c )
            return MG_FALSE;
    }
    return MG_TRUE;

}

char const* MgFindMatching(
    MgReader*   reader,
    char            c,
    int             count )
{
    for(;;)
    {
        char const* mark = reader->cursor;
        int d = MgGetChar(reader);
        if( d == -1 )
            return 0;
        if( d == c )
        {
            if( MgCheckMatch(reader, c, count-1) )
            {
                return mark;
            }
        }

        if( d == '\\' )
        {
            // read the escaped char, if any
            int e = MgGetChar(reader);
        }
    }
}

MgString MgFindMatchingString(
    MgReader*   reader,
    char            c,
    int             count)
{
    MgString text;
    text.begin = reader->cursor;
    text.end = MgFindMatching(reader, c, count);
    return text;
}

int MgGetLineNumber(
    MgInputFile*    inputFile,
    MgLine*         line)
{
    return 1 + (line - inputFile->beginLines);
}

int MgGetColumnNumber(
    MgLine*     line,
    const char* cursor)
{
    return 1 + (cursor - line->originalBegin);
}

MgSourceLoc MgGetSourceLoc(
    MgInputFile*    inputFile,
    MgLine*         line,
    const char*     cursor)
{
    MgSourceLoc sourceLoc;
    sourceLoc.line  = MgGetLineNumber( inputFile, line );
    sourceLoc.col   = MgGetColumnNumber( line, cursor );
    return sourceLoc;
}
