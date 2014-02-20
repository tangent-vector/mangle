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

typedef struct LineRangeT
{
    MgLine* begin;
    MgLine* end;
} LineRange;

MgElement* ReadSpansInRange(
    MgContext*      context,
    MgInputFile*    inputFile,
    LineRange       lines,
    MgSpanFlags       flags)
{
    return MgReadSpanElementsFromLines(
        context,
        inputFile,
        lines.begin,
        lines.end,
        flags );
}

MgElement* ReadElementsInRange(
    MgContext*      context,
    MgInputFile*    inputFile,
    LineRange       lineRange );

MgBool IsEmptyLine(
    MgLine* line )
{
    char const* cursor = line->text.begin;
    char const* end = line->text.end;
    while( cursor != end )
    {
        if( !isspace(*cursor) )
            return MG_FALSE;
        ++cursor;
    }
    return MG_TRUE;
}

enum
{
    kMaxHeaderLevel = 6,
};

MgBool LineIsAll(
    MgLine* line,
    char    c )
{
    char const* cursor  = line->text.begin;
    char const* end     = line->text.end;
    if( cursor == end )
        return MG_FALSE;
    while( cursor != end )
    {
        if( *cursor != c )
        {
            return MG_FALSE;
        }
        ++cursor;
    }
    return MG_TRUE;
}

typedef MgElement* (*ParseFunc)( MgContext*, MgInputFile*, LineRange* );

MgLine* GetLine(
    LineRange*  ioLineRange )
{
    if( ioLineRange->begin == ioLineRange->end )
        return 0;

    return ioLineRange->begin++;
}

void UnGetLine(
    LineRange*  ioLineRange,
    MgLine*     line )
{
    if( !line ) return;
    assert(line == (ioLineRange->begin-1));
    ioLineRange->begin--;
}

LineRange MgInclusiveLineRange(
    MgLine* first,
    MgLine* last)
{
    LineRange range = { first, last + 1 };
    return range;
}

LineRange Snip(
    MgLine*     firstLine,
    MgLine*     lastLine,
    LineRange*  ioLineRange )
{
    assert(firstLine);
    assert(lastLine);
    ioLineRange->begin = lastLine + 1;
    return MgInclusiveLineRange( firstLine, lastLine );
}

//
//
//


MgElement* ParseSetextHeader(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange,
    char            c,
    MgElementKind   kind )
{
    LineRange innerRange;
    MgLine* firstLine = GetLine(ioLineRange);
    MgLine* secondLine = GetLine(ioLineRange);
    if( !secondLine ) return 0;

    // ignore kind of first line, assuming other
    // parsers have gotten a fair chance already
    if(!LineIsAll(secondLine, c))
        return 0;

    // the inner range does not include the second line,
    // so we can't just use the Snip() function for everything
    innerRange = MgInclusiveLineRange(firstLine, firstLine);
    Snip( firstLine, secondLine, ioLineRange );

    MgElement* firstChild = ReadSpansInRange(
        context,
        inputFile,
        innerRange,
        kMgSpanFlags_Default );

    return MgCreateParentElement(
        kind,
        firstChild );
}

MgElement* ParseSetextHeader1(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseSetextHeader(
        context, inputFile,
        ioLineRange,
        '=',
        kMgElementKind_Header1 );
}

MgElement* ParseSetextHeader2(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseSetextHeader(
        context, inputFile,
        ioLineRange,
        '-',
        kMgElementKind_Header2 );
}

void TrimLeadingSpace(
    char const** ioBegin,
    char const* end )
{
    char const* begin = *ioBegin;
    while( begin != end && isspace(*begin) )
        ++begin;
    *ioBegin = begin;
}

void TrimTrailingSpace(
    char const* begin,
    char const** ioEnd )
{
    char const* end = *ioEnd;
    while( end != begin && isspace(*(end -1)) )
        --end;
    *ioEnd = end;
}

void TrimTrailingChar(
    char const* begin,
    char const** ioEnd,
    char            c )
{
    char const* end = *ioEnd;
    while( end != begin && (*(end -1) == c) )
        --end;
    *ioEnd = end;
}

void InitializeLineReader(
    MgReader*   reader,
    MgLine*         line )
{
    MgInitializeStringReader( reader, line->text );
}

MgElement* ParseAtxHeader(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    LineRange innerRange;
    MgLine* firstLine = GetLine( ioLineRange );

    MgReader reader;
    InitializeLineReader(&reader, firstLine);

    int level = 0;
    for(;;)
    {
        int c = MgGetChar(&reader);
        if( c != '#' )
        {
            MgUnGetChar(&reader, c);
            break;
        }
        ++level;
    }

    if( level == 0 ) return 0;

    firstLine->text.begin = reader.cursor;

    TrimLeadingSpace( &firstLine->text.begin, firstLine->text.end );
    TrimTrailingChar( firstLine->text.begin, &firstLine->text.end, '#' );
    TrimTrailingSpace( firstLine->text.begin, &firstLine->text.end );

    if( level > kMaxHeaderLevel )
        level = kMaxHeaderLevel;

    innerRange = Snip( firstLine, firstLine, ioLineRange );

    MgElement* firstChild = ReadSpansInRange( context, inputFile, innerRange, kMgSpanFlags_Default );

    return MgCreateParentElement(
        (MgElementKind) (kMgElementKind_Header1 + (level-1)),
        firstChild );
}

char const* CheckQuoteLine(
    MgLine* line )
{
    MgReader reader;
    InitializeLineReader(&reader, line );

    int c = MgGetChar( &reader );
    if( c != '>' )
        return 0;

    int d = MgGetChar( &reader );
    if( d != ' ' )
    {
        MgUnGetChar( &reader, d );
    }
    return reader.cursor;
}

MgElement* ParseBlockQuote(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    MgLine* firstLine = GetLine( ioLineRange );
    MgLine* lastLine = firstLine;

    MgLine* line = firstLine;
    for(;;)
    {
        if( !line )
            break;

        char const* lineStart = CheckQuoteLine(line);
        if( !lineStart )
        {
            if( line == firstLine )
                return 0; // this isn't a block quote at all!
            else
                break; // we've found the first line that doesn't belong
        } 

        // we are starting a paragraph within the block quote

        // continue consuming lines until we see an empty line
        while( line && !IsEmptyLine(line) )
        {
            lineStart = CheckQuoteLine(line);
            if( lineStart )
                line->text.begin = lineStart;
            lastLine = line;
            line = GetLine( ioLineRange );
        }

        // continue consuming lines until we see a non-empty line
        while( line && IsEmptyLine(line) )
        {
            line = GetLine( ioLineRange );
        }
    }

    LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );

    MgElement* firstChild = ReadElementsInRange(context, inputFile, innerRange);
    return MgCreateParentElement(
        kMgElementKind_BlockQuote,
        firstChild );
}

int GetIndent(
    MgLine* line )
{
    char const* cursor = line->text.begin;
    char const* end = line->text.end;

    int indent = 0;
    while( cursor != end )
    {
        char c = *cursor;
        if( c == ' ' )
        {
            indent += 1;
        }
        else if( c == '\t' )
        {
            indent += 4;
        }
        else
        {
            break;
        }
        ++cursor;
    }
    return indent;
}

MgString CString(char const* text)
{
    MgString string = { text, text + strlen(text) };
    return string;
}

static void SkipWhiteSpace(
    MgReader* reader )
{
    for(;;)
    {
        int c = MgGetChar(reader);
        if( !isspace(c) )
        {
            MgUnGetChar(reader, c);
            return;
        }
    }
}

MgBool ParseLiterateScrapIntroduction(
    MgContext*      context,
    MgInputFile*    inputFile,
    MgLine*         line,
    MgScrapKind*      outScrapKind,
    char const**    outScrapIdBegin,
    char const**    outScrapIdEnd,
    char const**    outScrapNameBegin,
    char const**    outScrapNameEnd )
{
    MgReader reader;
    InitializeLineReader( &reader, line );

    // Allow scrap introduction to start with
    // comment and whitespace:
    int c = MgGetChar( &reader );
    switch( c )
    {
    default:
        MgUnGetChar( &reader, c );
        break;

    case '/':
        {
            int d = MgGetChar( &reader );
            if( d != '/' )
            {
                MgUnGetChar( &reader, d );
                MgUnGetChar( &reader, c );
                break;
            }

            SkipWhiteSpace( &reader );
            break;
        }

    // TODO: C-style "/* */" comments
    }

    if( MgGetChar(&reader) != '<' )
        return MG_FALSE;
    if( MgGetChar(&reader) != '<' )
        return MG_FALSE;

    // look for a possible scrap kind marker
    char const* savedCursor = reader.cursor;
    char const* scrapKindBegin = savedCursor;
    char const* scrapKindEnd = 0;
    for( ;; )
    {
        int c = MgGetChar( &reader );
        if( c == ':' )
        {
            scrapKindEnd = reader.cursor-1;
            break;
        }

        if( !isalpha(c) )
            break;
    }

    MgScrapKind scrapKind = kScrapKind_Unknown;
    if( scrapKindEnd )
    {
        // look for a kind
        MgString scrapKindName = { scrapKindBegin, scrapKindEnd };
        if( MgStringsAreEqual(scrapKindName, CString("file")) )
        {
            scrapKind = kScrapKind_OutputFile;
        }
        else if( MgStringsAreEqual(scrapKindName, CString("global")) )
        {
            scrapKind = kScrapKind_GlobalMacro;
        }
        else if( MgStringsAreEqual(scrapKindName, CString("local")) )
        {
            scrapKind = kScrapKind_LocalMacro;
        }
        else
        {
            fprintf(stderr, "unknown scrap kind: \"");
            fwrite(scrapKindName.begin, 1, scrapKindName.end - scrapKindName.begin, stderr);
            fprintf(stderr, "\"\n");
        }
    }
    else
    {
        reader.cursor = savedCursor;
    }

    char const* scrapIdBegin = reader.cursor;

    for(;;)
    {
        int c = MgGetChar(&reader);
        if( c == -1 ) break;

        if( c == '|' )
        {
            MgUnGetChar(&reader, c);
            break;
        }

        if( c == '>' )
        {
            int d = MgGetChar(&reader);
            MgUnGetChar(&reader, d );
            if( d == '>' )
            {
                MgUnGetChar(&reader, c);
                break;
            }
        }
    }

    char const* scrapIdEnd = reader.cursor;
    char const* scrapNameBegin = 0;
    char const* scrapNameEnd = 0;

    int pipe = MgGetChar(&reader);
    if( pipe != '|' )
    {
        MgUnGetChar(&reader, pipe);
    }
    else
    {
        scrapNameBegin = reader.cursor;
        for(;;)
        {
            int c = MgGetChar(&reader);
            if( c == -1 ) break;
            if( c == '>' )
            {
                int d = MgGetChar(&reader);
                MgUnGetChar(&reader, d);
                if( d == '>' )
                {
                    MgUnGetChar(&reader, c);
                    break;
                }
            }
        }
        scrapNameEnd = reader.cursor;
    }

    if( MgGetChar(&reader) != '>' )
        return MG_FALSE;
    if( MgGetChar(&reader) != '>' )
        return MG_FALSE;

    TrimLeadingSpace(&scrapIdBegin, scrapIdEnd);
    TrimTrailingSpace(scrapIdBegin, &scrapIdEnd);

    TrimLeadingSpace(&scrapNameBegin, scrapNameEnd);
    TrimTrailingSpace(scrapNameBegin, &scrapNameEnd);

    *outScrapKind       = scrapKind;
    *outScrapIdBegin    = scrapIdBegin;
    *outScrapIdEnd      = scrapIdEnd;
    *outScrapNameBegin  = scrapNameBegin;
    *outScrapNameEnd    = scrapNameEnd;

    return MG_TRUE;
}


MgElement* ParseCodeBlockBody(
    MgContext*      context,
    MgInputFile*    inputFile,
    LineRange       inLineRange,
    char const*     langBegin,
    char const*     langEnd )
{
    // check if first line looks like a
    // literate scrap introduction
    LineRange lineRange = inLineRange;
    MgLine* firstLine = GetLine( &lineRange );

    MgScrapKind scrapKind = kScrapKind_Unknown;
    char const* scrapIdBegin    = 0;
    char const* scrapIdEnd      = 0;
    char const* scrapNameBegin  = 0;
    char const* scrapNameEnd    = 0;
    if( ParseLiterateScrapIntroduction(
        context, inputFile, firstLine,
        &scrapKind,
        &scrapIdBegin,      &scrapIdEnd,
        &scrapNameBegin,    &scrapNameEnd ) )
    {
        firstLine = firstLine + 1;
    }
    else
    {
        UnGetLine( &lineRange, firstLine );
    }

    MgElement* firstChild = ReadSpansInRange( context, inputFile, lineRange, kMgSpanFlags_CodeBlock );
    MgElement* codeBlock = MgCreateParentElement(
        kMgElementKind_CodeBlock,
        firstChild );

    if( langBegin != langEnd )
    {
        MgAddAttribute( codeBlock, "class", MgMakeString( langBegin, langEnd ) );
    }

    MgElement* element = codeBlock;
    if( scrapIdBegin != 0 )
    {
        MgString scrapID = { scrapIdBegin, scrapIdEnd };
        MgString scrapName = { scrapNameBegin, scrapNameEnd };
        MgScrapFileGroup* scrapGroup = MgFindOrCreateScrapGroup(
            context,
            scrapKind,
            scrapID,
            inputFile );
        if( scrapName.begin != scrapName.end )
        {
            // \todo: check that we are the first!!!
            scrapGroup->nameGroup->name = MgReadSpanElements(context, inputFile, firstLine, scrapName, kMgSpanFlags_Default);
        }

        MgScrap* scrap = (MgScrap*) malloc(sizeof(MgScrap));
        scrap->fileGroup = scrapGroup;
        scrap->sourceLoc = MgGetSourceLoc( inputFile, firstLine, firstLine->text.begin );
        scrap->body = codeBlock;
        scrap->next = 0;
        
        MgAddScrapToFileGroup( scrapGroup, scrap );

        element = MgCreateParentElement(
            kMgElementKind_ScrapDef,
            element );

        MgAttribute* attr = MgAddCustomAttribute( element, "$scrap" );
        attr->scrap = scrap;
    }


    return element;
}

char const* CheckIndentedCodeLine(
    MgLine* line )
{
    // either a tab or four spaces
    MgReader reader;
    InitializeLineReader(&reader, line );

    int c = MgGetChar( &reader );
    if( c == '\t' )
        return reader.cursor;
    MgUnGetChar( &reader, c );

    for( int ii = 0; ii < 4; ++ii )
    {
        int d = MgGetChar( &reader );
        if( d != ' ' )
            return 0;
    }
    return reader.cursor;
}

MgElement* ParseIndentedCode(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    MgLine* firstLine = GetLine(ioLineRange);
    MgLine* lastLine = firstLine;

    MgLine* line = firstLine;
    for(;;)
    {
        if( !line )
            break;

        char const* lineStart = CheckIndentedCodeLine(line);
        if( !lineStart )
        {
            if( line == firstLine )
                return 0; // this isn't a code block at all!
            else
                break; // end of the code block
        }

        // we are starting a paragraph within the code block

        // continue consuming lines until we see an empty line
        while( line && !IsEmptyLine(line) )
        {
            lineStart = CheckIndentedCodeLine(line);
            if(!lineStart)
                break; // end of the code element...

            line->text.begin = lineStart;
            lastLine = line;
            line = GetLine( ioLineRange );
        }

        // continue consuming lines until we see a non-empty line
        while( line && IsEmptyLine(line) )
        {
            line = GetLine(ioLineRange);
        }
    }

    LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );

    return ParseCodeBlockBody(
        context,
        inputFile,
        innerRange,
        0, 0 ); // no way to pass in a language name
}

char const* CheckBracketedCodeLine(
    MgLine* line,
    char    c )
{
    MgReader reader;
    InitializeLineReader( &reader, line );

    for( int ii = 0; ii < 3; ++ii )
    {
        int d = MgGetChar( &reader );
        if( d != c )
            return 0;
    }
    return reader.cursor;
}

MgElement* ParseBracketedCode(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange,
    char c )
{
    MgLine* openLine = GetLine( ioLineRange );
    char const* startLang = CheckBracketedCodeLine( openLine, c );
    if( !startLang )
        return 0;
    openLine->text.begin = startLang;

    MgLine* firstLine = 0;
    MgLine* lastLine = 0;

    for(;;)
    {
        MgLine* line = GetLine( ioLineRange );
        if( !line )
            break; // \todo: this would make an un-terminated code block...
        char const* end = CheckBracketedCodeLine( line, c );
        if( end )
        {
            // \todo: what if there is text after the backticks?
            // for now, we process it as an other text...
            line->text.begin = end;

            break;
        }

        if( !firstLine )
            firstLine = line;
        lastLine = line;
    }

    // the span *might* be empty... just in case
    LineRange innerRange = { 0, 0 };
    if( lastLine )
        innerRange = Snip( firstLine, lastLine, ioLineRange );

    // `openLine` begin/end gives us the language marker, if any
    return ParseCodeBlockBody(
        context,
        inputFile,
        innerRange,
        openLine->text.begin,
        openLine->text.end );
}

MgElement* ParseBracketedCode_Backtick(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseBracketedCode( context, inputFile, ioLineRange, '`' );
}

MgElement* ParseBracketedCode_Tilde(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseBracketedCode( context, inputFile, ioLineRange, '~' );
}

MgElement* ParseHorizontalRule(
    MgContext*      context,
    MgInputFile*    inputFile,
    LineRange*      ioLineRange,
    char            c )
{
    MgLine* firstLine = GetLine(ioLineRange);

    MgReader reader;
    InitializeLineReader(&reader, firstLine);

    int count = 0;
    for(;;)
    {
        int d = MgGetChar(&reader);

        if( d == -1 )
            break;

        if( d == c )
        {
            ++count;
        }
        else if( isspace(d) )
        {}
        else
        {
            return 0;
        }
    }

    if( count < 3 )
        return 0;

    Snip( firstLine, firstLine, ioLineRange );

    return MgCreateLeafElement(
        kMgElementKind_HorizontalRule,
        MgMakeString(MG_NULL, MG_NULL) );
}

MgElement* ParseHorizontalRule_Hypen(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseHorizontalRule( context, inputFile, ioLineRange, '-' );
}

MgElement* ParseHorizontalRule_Asterisk(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseHorizontalRule( context, inputFile, ioLineRange, '*' );
}

MgElement* ParseHorizontalRule_Underscore(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseHorizontalRule( context, inputFile, ioLineRange, '_' );
}

char const* CheckUnorderedListLine(
    MgLine* line )
{
    MgReader reader;
    InitializeLineReader( &reader, line );

    // may have up to three leading spaces
    for(int ii = 0; ii < 3; ++ii )
    {
        int cc = MgGetChar( &reader );
        if( cc != ' ' )
        {
            MgUnGetChar( &reader, cc );
            break;
        }
    }

    int c = MgGetChar( &reader );
    switch( c )
    {
    case '*':
    case '+':
    case '-':
        break;

    default:
        return 0;
    }

    // skip white-space after the bullet
    for(;;)
    {
        int d = MgGetChar( &reader );
        if( !isspace(d) )
        {
            MgUnGetChar( &reader, d );
            break;
        }
    }

    return reader.cursor;
}

char const* CheckOrderedListLine(
    MgLine* line )
{
    MgReader reader;
    InitializeLineReader( &reader, line );

    // may have up to three leading spaces
    for(int ii = 0; ii < 3; ++ii )
    {
        int cc = MgGetChar( &reader );
        if( cc != ' ' )
        {
            MgUnGetChar( &reader, cc );
            break;
        }
    }

    int c = MgGetChar( &reader );
    if( !isdigit(c) )
        return 0;

    // skip remaining digits
    for(;;)
    {
        int d = MgGetChar( &reader );
        if( !isdigit(d) )
        {
            MgUnGetChar( &reader, d );
            break;
        }
    }

    // expect a dot
    int e = MgGetChar( &reader );
    if( e != '.' )
        return 0;

    // skip white-space after the dot
    for(;;)
    {
        int d = MgGetChar( &reader );
        if( !isspace(d) )
        {
            MgUnGetChar( &reader, d );
            break;
        }
    }

    return reader.cursor;
}

char const* CheckListLineLeadingSpace(
    MgLine* line )
{
    MgReader reader;
    InitializeLineReader( &reader, line );

    for( int ii = 0; ii < 4; ++ii )
    {
        int c = MgGetChar( &reader );
        if( c == '\t' )
            break;
        if( c != ' ' )
        {
            MgUnGetChar( &reader, c );
            return 0;
        }
    }
    return reader.cursor;
}

MgElement* ParseListItem(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange,
    char const* (*checkLineFunc)(MgLine*) )
{
    MgLine* firstLine = GetLine( ioLineRange );
    MgLine* lastLine = firstLine;

    if( !firstLine )
        return 0;

    // check the first line
    char const* lineStart = checkLineFunc(firstLine);
    if( !lineStart )
        return 0;
    firstLine->text.begin = lineStart;

    // read subsequent lines of the item, until
    MgLine* line = firstLine;

    for(;;)
    {
        line = GetLine( ioLineRange );
        while( line )
        {
            // we see an empty line, or
            if( IsEmptyLine(line) )
                break;

            // a line that starts a new item
            // \todo: does this need to consider other list flavors?
            lineStart = checkLineFunc(line);
            if(lineStart)
                break;

            lineStart = CheckListLineLeadingSpace(line);
            if( lineStart )
                line->text.begin = lineStart;

            // \todo: need to trim front of line...

            lastLine = line;
            line = GetLine(ioLineRange);
        }

        // continue consuming lines until we see a non-empty line
        while( line && IsEmptyLine(line) )
        {
            line = GetLine(ioLineRange);
        }

        // if the next line is indented appropriately for line
        // continuation... we continue building out the same item...
        if( line )
        {
            lineStart = CheckIndentedCodeLine( line );
            if( lineStart )
            {
                line->text.begin = lineStart;
                lastLine = line;
                continue;
            }
        }
        break;
    }

    LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );

    MgElement* firstChild = ReadElementsInRange( context, inputFile, innerRange );

    return MgCreateParentElement(
        kMgElementKind_ListItem,
        firstChild );
}

MgElement* ParseList(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange,
    MgElementKind kind,
    char const* (*checkLineFunc)(MgLine*) )
{
    MgElement* firstItem = ParseListItem( context, inputFile, ioLineRange, checkLineFunc );
    MgElement* lastItem = firstItem;

    if( !firstItem ) return 0;

    for(;;)
    {
        MgElement* item = ParseListItem( context, inputFile, ioLineRange, checkLineFunc );
        if( !item )
            break;

        lastItem->next = item;
        lastItem = item;
    }

    return MgCreateParentElement(
        kind,
        firstItem );
}

MgElement* ParseOrderedList(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseList(
        context, inputFile, 
        ioLineRange,
        kMgElementKind_OrderedList,
        &CheckOrderedListLine );
}

MgElement* ParseUnorderedList(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    return ParseList(
        context, inputFile,
        ioLineRange,
        kMgElementKind_UnorderedList,
        &CheckUnorderedListLine );
}

void SkipEmptyLines(
    LineRange*  ioLineRange )
{
    for(;;)
    {
        MgLine* begin = ioLineRange->begin;
        if( begin == ioLineRange->end )
            return;
        if( !IsEmptyLine(begin) )
            return;
        
        ioLineRange->begin = begin + 1;
    }
}

MgElement* ParseDefaultParagraph(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    // an ordinary text paragraph will consume
    // everything up to the next blank line...

    MgLine* firstLine = GetLine( ioLineRange );

    MgLine* lastLine = firstLine;
    for(;;)
    {
        MgLine* line = GetLine( ioLineRange );
        if( !line || IsEmptyLine(line) )
        {
            UnGetLine(ioLineRange, line);
            break;
        }

        lastLine = line;
    }

    LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );

    MgElement* firstChild = ReadSpansInRange( context, inputFile, innerRange, kMgSpanFlags_Default );
    return MgCreateParentElement(
        kMgElementKind_Paragraph,
        firstChild );
}

MgElement* ParseBlockLevelHtml(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    MgLine* firstLine = GetLine( ioLineRange );
    MgLine* lastLine = firstLine;

    MgReader reader;
    InitializeLineReader( &reader, firstLine );

    int c = MgGetChar( &reader );
    if( c != '<' ) return MG_NULL;

    int d = MgGetChar( &reader );
    if( !isalpha(d) ) return MG_NULL;

    for(;;)
    {
        MgLine* line = GetLine( ioLineRange );
        if( !line )
            break;

        lastLine = line;

        InitializeLineReader( &reader, line );
        int e = MgGetChar( &reader );
        if( e != '<' )
            continue;
        int f = MgGetChar( &reader );
        if( f != '/' )
            continue;
        int g = MgGetChar( &reader );
        if( !isalpha(g) )
            continue;

        break;
    }

    LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );

    MgElement* firstChild = ReadSpansInRange( context, inputFile, innerRange, kMgSpanFlags_HtmlBlock );
    return MgCreateParentElement(
        kMgElementKind_HtmlBlock,
        firstChild );
}

MgBool ParseLinkDefinitionTitle(
    MgReader*   reader,
    char const**    outTitleBegin,
    char const**    outTitleEnd )
{
    int openTitle = MgGetChar(reader);
    int closeTitle;
    switch(openTitle)
    {
    default:
        return MG_FALSE;
    case '"':
    case '\'':
        closeTitle = openTitle;
        break;
    case '(':
        closeTitle = ')';
        break;
    }

    char const* titleBegin = reader->cursor;
    char const* titleEnd = MgFindMatching(reader, closeTitle, 1);
    if( !titleEnd )
        return MG_FALSE;

    // \todo: enforce that we find end of line?

    *outTitleBegin = titleBegin;
    *outTitleEnd = titleEnd;
    return MG_TRUE;
}

MgElement* ParseLinkDefinition(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    MgString id;
    MgLine* firstLine = GetLine(ioLineRange);

    MgReader reader;
    InitializeLineReader(&reader, firstLine);

    // skip up to three spaces
    for(int ii = 0; ii < 3; ++ii)
    {
        int c = MgGetChar(&reader);
        if( c != ' ' )
        {
            MgUnGetChar(&reader, c);
            break;
        }
    }

    // expect a '['
    int openBrace = MgGetChar(&reader);
    if( openBrace != '[' )
        return 0;

    id = MgFindMatchingString(&reader, ']', 1);
    if( !id.end )
        return 0;

    // expect a ':'
    int colon = MgGetChar(&reader);
    if( colon != ':' )
        return 0;

    // skip any white space
    SkipWhiteSpace(&reader);

    // allow one '<' before the link
    int leftAngle = MgGetChar(&reader);
    if( leftAngle != '<' )
    {
        MgUnGetChar(&reader, leftAngle);
    }

    // read the URL (assume it continues until white-space)
    char const* urlBegin = reader.cursor;
    char const* urlEnd = reader.cursor;
    for(;;)
    {
        int c = MgGetChar( &reader );
        if( c == -1 || c == '>' || isspace(c) )
        {
            MgUnGetChar(&reader, c);
            break;
        }
        urlEnd = reader.cursor;
    }

    // expect an opening '<' to be matched by '>'
    if( leftAngle == '<' )
    {
        int rightAngle = MgGetChar( &reader );
        if( rightAngle != '>' )
            return 0;
    }

    // \todo: should we check that the URL is non-empty?

    SkipWhiteSpace(&reader);

    // now look for the title
    char const* titleBegin = 0;
    char const* titleEnd = 0;
    if( !MgAtEnd(&reader) )
    {
        ParseLinkDefinitionTitle(
            &reader,
            &titleBegin,
            &titleEnd );
    }
    else
    {
        MgLine* secondLine = GetLine(ioLineRange);
        InitializeLineReader(&reader, secondLine);
        SkipWhiteSpace(&reader);

        if( !ParseLinkDefinitionTitle(
            &reader,
            &titleBegin,
            &titleEnd ) )
        {
            UnGetLine(ioLineRange, secondLine);
        }
    }

    MgReferenceLink* ref = MgFindOrCreateReferenceLink(
        inputFile,
        id );

    // \todo: how to handle redefinition?
    ref->url.begin = urlBegin;
    ref->url.end = urlEnd;
    ref->title.begin = titleBegin;
    ref->title.end = titleEnd;

    // we have to return a non-NULL element to indicate
    // a successful parse...
    return MgCreateLeafElement(
        kMgElementKind_Text,
        MgMakeString(MG_NULL, MG_NULL));
}

int CountTableLinePipes(
    MgLine*   line)
{
    MgReader reader;
    InitializeLineReader(&reader, line);

    int pipeCount = 0;
    for( ;; )
    {
        int c = MgGetChar(&reader);
        if( c == -1 )
            break;
        else if( c == '|' )
            ++pipeCount;
        else if( c == '\\' )
            MgGetChar(&reader);
    }
    return pipeCount;
}

MgElement* ParseTableCell(
    MgContext*      context,
    MgInputFile*    inputFile,
    MgLine*         line,
    MgReader*   reader,
    MgElementKind   kind )
{
    char const* cellBegin   = reader->cursor;
    char const* cellEnd     = cellBegin;
    for( ;; )
    {
        cellEnd = reader->cursor;
        int c = MgGetChar( reader );
        if( c == '|' || c == -1 )
        {
            MgUnGetChar(reader, c);
            break;
        }

        // allow escapes?
        if( c == '\\' )
        {
            int d = MgGetChar(reader);
        }
    }

    MgElement* firstChild = MgReadSpanElements(context, inputFile, line, MgMakeString(cellBegin, cellEnd), kMgSpanFlags_Default);
    return MgCreateParentElement(
        kind,
        firstChild );
}

MgBool ParseTableAlignments(
    MgLine* line)
{
    MgReader reader;
    InitializeLineReader(&reader, line);

    int pipeCount = 0;
    int columnCount = 0;
    for( ;; )
    {
        MgBool leftJustify = MG_FALSE;
        MgBool rightJustify = MG_FALSE;

        if( MgPeekChar(&reader) == '|' )
        {
            MgGetChar(&reader);
            ++pipeCount;
        }

        if( MgPeekChar(&reader) == -1 )
            break;

        ++columnCount;
        if( MgPeekChar(&reader) == ':' )
        {
            MgGetChar(&reader);
            leftJustify = MG_TRUE;
        }

        while( MgPeekChar(&reader) == '-' )
            MgGetChar(&reader);

        if( MgPeekChar(&reader) == ':' )
        {
            MgGetChar(&reader);
            rightJustify = MG_TRUE;
        }

        int next = MgPeekChar(&reader);
        if( next != -1 && next != '|' )
            return MG_FALSE;
    }

    return pipeCount != 0;
}

MgElement* ParseTableRow(
    MgContext*      context,
    MgInputFile*    inputFile,
    MgLine*         line,
    MgElementKind   cellKind )
{
    MgReader reader;
    InitializeLineReader( &reader, line );

    // start reading cells
    MgElement* firstCell = 0;
    MgElement* lastCell = 0;
    for(;;)
    {
        // note that this treats a pipe at the start
        // of the line as always being a decorative pipe
        if( MgPeekChar(&reader) == '|' )
        {
            MgGetChar(&reader);
        }

        // end of line?
        //
        // note that this always treats a pipe at the end
        // of the line as a decorative pipe
        if( MgPeekChar(&reader) == -1 )
            break;

        MgElement* cell = ParseTableCell(context, inputFile, line, &reader, cellKind);
        if( lastCell )
        {
            lastCell->next = cell;
        }
        else
        {
            firstCell = cell;
        }
        lastCell = cell;
    }

    return MgCreateParentElement(
        kMgElementKind_TableRow,
        firstCell );
}

MgElement* ParseTable(
    MgContext*    context,
    MgInputFile*  inputFile,
    LineRange*  ioLineRange )
{
    MgLine* headerLine      = GetLine( ioLineRange );
    MgLine* alignmentLine   = MG_NULL;
    MgLine* firstLine       = MG_NULL;
    MgLine* lastLine        = MG_NULL;
    LineRange innerRange;

    int headerPipes = CountTableLinePipes( headerLine );
    if( headerPipes == 0 )
        return 0;

    // \todo: how to do table cell alignment?
    alignmentLine = GetLine( ioLineRange );
    if( !alignmentLine || !ParseTableAlignments(alignmentLine) )
        return 0;

    // find successive lines that might be table rows
    for(;;)
    {
        MgLine* line = GetLine( ioLineRange );
        if( !line )
            break;

        int linePipes = CountTableLinePipes( line );
        if( linePipes == 0 )
        {
            UnGetLine(ioLineRange, line);
            break;
        }
        if( !firstLine )
            firstLine = line;
        lastLine = line;
    }
    // we need at least one line
    if( !firstLine )
        return 0;

    innerRange = Snip( firstLine, lastLine, ioLineRange );

    MgElement* firstRow = ParseTableRow(context, inputFile, headerLine, kMgElementKind_TableHeader);
    MgElement* lastRow = firstRow;
    for( MgLine* line = innerRange.begin; line != innerRange.end; ++line )
    {
        MgElement* row = ParseTableRow(context, inputFile, line, kMgElementKind_TableCell);
        lastRow->next = row;
        lastRow = row;
    }

    return MgCreateParentElement(
        kMgElementKind_Table,
        firstRow );
}

MgElement* ParseMetaData(
    MgContext*      context,
    MgInputFile*    inputFile,
    LineRange*      ioLineRange )
{
    MgLine* firstLine = GetLine( ioLineRange );
    if( !firstLine )
        return MG_NULL;

    MgReader reader;
    InitializeLineReader( &reader, firstLine );

    // meta-data line can't start with whitespace
    if( isspace(MgPeekChar(&reader)) )
        return MG_NULL;

    // read until a ':'
    MgString key = MgFindMatchingString(&reader, ':', 1);
    if( !key.end )
        return MG_NULL;

    MgString value = MgMakeString(reader.cursor, firstLine->text.end);
    TrimTrailingSpace(value.begin, &value.end);
    TrimLeadingSpace(&value.begin, value.end);

    MgElement* firstChild = MgCreateLeafElement(kMgElementKind_Text, value);
    MgElement* lastChild = firstChild;

    for(;;)
    {
        MgLine* line = GetLine( ioLineRange );
        if( !line )
            break;

        InitializeLineReader( &reader, line );
        if( !isspace(MgPeekChar(&reader)) )
        {
            UnGetLine( ioLineRange, line );
            break;
        }

        value = line->text;
        TrimTrailingSpace(value.begin, &value.end);
        TrimLeadingSpace(&value.begin, value.end);

        MgElement* child = MgCreateLeafElement(kMgElementKind_Text, value);
        lastChild->next = child;
        lastChild = child;
    }

    MgElement* element = MgCreateParentElement(
        kMgElementKind_MetaData,
        firstChild );
    MgAddAttribute(element, "$key", key);
    return element;
}

MgElement* ReadElement(
    MgContext*      context,
    MgInputFile*    inputFile,
    LineRange*      ioLineRange )
{
    // earlier code has skipped any empty lines,
    // so we know this line represents the start
    // of some kind of block-level structure.

    static const ParseFunc parseFuncs[] = {
        &ParseLinkDefinition,
        &ParseTable,
        &ParseBlockLevelHtml,
        &ParseBlockQuote,
        &ParseIndentedCode,
        &ParseBracketedCode_Backtick,
        &ParseBracketedCode_Tilde,
        &ParseAtxHeader,
        // check for rules before lists, since they
        // would match otherwise...
        &ParseHorizontalRule_Hypen,
        &ParseHorizontalRule_Asterisk,
        &ParseHorizontalRule_Underscore,
        &ParseOrderedList,
        &ParseUnorderedList,
        // check for the setext headers late, since
        // they don't pay attention to the text on
        // the first line
        &ParseSetextHeader1,
        &ParseSetextHeader2,
        // check for ordinary text paragraphs last,
        // so that we always have a fallback
        &ParseDefaultParagraph,
    };

    ParseFunc const* funcCursor = &parseFuncs[0];
    for(;;)
    {
        LineRange lineRange = *ioLineRange;
        MgElement* p = (*funcCursor)( context, inputFile, &lineRange );
        if( p )
        {
            *ioLineRange = lineRange;            
            return p;
        }
        ++funcCursor;
    }
}

MgElement* MgParseBlockElements(
    MgContext*      context,
    MgInputFile*    inputFile,
    MgLine*         beginLines,
    MgLine*         endLines )
{
    // TODO: parse meta-data elements until no more matches

    MgElement* firstElement = MG_NULL;
    MgElement* lastElement  = MG_NULL;

    LineRange lineRange = { beginLines, endLines };
    firstElement = ReadElement( context, inputFile, &lineRange );
    lastElement = firstElement;
    for(;;)
    {
        SkipEmptyLines(&lineRange);
        if( lineRange.begin == lineRange.end )
            break;

        MgElement* element = ReadElement( context, inputFile, &lineRange );
        lastElement->next = element;
        lastElement = element;
    }

    return firstElement;
}

MgElement* ReadElementsInRange(
    MgContext*      context,
    MgInputFile*    inputFile,
    LineRange       lineRange)
{
    return MgParseBlockElements(
        context,
        inputFile,
        lineRange.begin,
        lineRange.end );
}

MgElement* MgParseMetaDataElements(
    MgContext*      context,
    MgInputFile*    inputFile,
    MgLine*         beginLines,
    MgLine*         endLines )
{
    MgElement* firstElement = MG_NULL;
    MgElement* lastElement  = MG_NULL;

    LineRange lineRange = { beginLines, endLines };
    for(;;)
    {
        SkipEmptyLines(&lineRange);
        if( lineRange.begin == lineRange.end )
            break;

        LineRange savedLineRange = lineRange;
        MgElement* element = ParseMetaData(context, inputFile, &lineRange);
        if( !element )
        {
            fprintf(stderr, "bad metadata line\n");
            // if we failed to parse meta-data, skip a line and keep trying
            lineRange = savedLineRange;
            ++lineRange.begin;            
            continue;
        }

        if( lastElement )
        {
            lastElement->next = element;
        }
        else
        {
            firstElement = element;
        }
        lastElement = element;
    }

    return firstElement;    
}