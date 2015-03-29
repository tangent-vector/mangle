Parsing Block-Level Elements
============================

This section includes many block quotes from the original Markdown [syntax][Markdown:Syntax] reference.

[Markdown:Syntax]: http://daringfireball.net/projects/markdown/syntax

As much as possible, we try to follow the order of presentation in that document.

Overview
--------

When it comes time to parse block-level element the input has already been parsed into an array of `Line` structures.
Every block-level element in Markdown will correspond to one or more lines of input.
The core of our approach will come down to two functions.
The first, `ParseBlockElement` will parse the first block-level element from a range of lines, and modify the range to point after the element.
The second, `ParseBlockElements` will parse then loop to parse all of the block-level elements in a range of lines.
In each case, the lines are expected to come from the same `InputFile`.

    <<block-level parsing declarations>>=
    MgElement* ParseBlockElement(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange*      ioLineRange );

    MgElement* ParseBlockElementsInRange(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       lineRange );

Since the lines of each input file are stored in a contiguous array, we can represent a range of lines in a file as a pair of a `begin` and `end` line pointer.

    <<block-level parsing types>>+=
    typedef struct LineRangeT
    {
        MgLine* begin;
        MgLine* end;
    } LineRange;

### Parsing All Elements in a Range ###

In order to parse a range of lines into block-level elements, we will repeatedly try to read one element and append it to a running list.
We maintain both the pointer to the "head" of the list, as well as a "link" pointer which will point at the terminating `NULL`.

    <<block-level parsing definitions>>=
    MgElement* ParseBlockElementsInRange(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       lineRange )
    {
        MgElement*  elements    = NULL;
        MgElement** elementLink = &elements;

        for(;;)
        {
            <<try to read and append one block-level element>>   
        }

        return elements;
    }

When trying to read an element, we first skip any blank lines at the start of the input range.
If this brings us to the end of the input, then we are done parsing elements and we `break` out of the loop.
Otherwise we read an element and append it to our list.

    <<try to read and append one block-level element>>=
    SkipEmptyLines(&lineRange);
    <<`break` if out of input>>
    <<read one block-level element>>
    <<append block-level element to the list>>

Checking that we are out of input is as simple as seeing if the `begin` and `end` of the range are equal.

    <<`break` if out of input>>=
    if( lineRange.begin == lineRange.end )
        break;

Reading a block-level element is handled by the `ParseBlockElement()` function, which will be covered next.

    <<read one block-level element>>=
    MgElement* element = ParseBlockElement( context, inputFile, &lineRange );

The `ReadBlockElement()` function might in some cases return a list of elements, so after we link `element` onto the end of our list, we keep traversing through the list, updating `elementLink` until we get to the end of the newly-extended list (that is, when `*elementLink` is null).

    <<append block-level element to the list>>=
    *elementLink = element;
    while( *elementLink )
        elementLink = &(*elementLink)->next;


### Parsing One Element ###

Our approach to parsing a single element will be very simple, at the possible cost of some performance.
In order to identify the right kind of element to parse, we will try each of the possibilities in turn, and use the first one that successfully matches the input.

As yet another simplification, we will use a single function signature, `BlockParseFunc` for each of the element kinds, so that we can construct a table of function pointers.
Each implementation of this signature will return a parsed `Element` on success, or `NULL` on failure.
The `BLOCK_PARSE_FUNC` macro simplifies declaring a function with this signature.

    <<block-level parsing types>>+=
    #define BLOCK_PARSE_FUNC(Name) \
        MgElement* Name( MgContext* context, MgInputFile* inputFile, LineRange* ioLineRange )
    typedef BLOCK_PARSE_FUNC((*BlockParseFunc));

The `ParseBlockElement` function itself corresponds to this signature.
It iterates over the a table of pointers to all of our block-level parsing functions, until it finds one that matches the input.
At that point it updates the line range, and returns the match.


    <<`ParseBlockElement` function>>=
    BLOCK_PARSE_FUNC(ParseBlockElement)
    {
        static const BlockParseFunc kBlockParseFuncs[] = {
            <<block-level parsing function pointers>>
        };

        BlockParseFunc const* funcCursor = &kBlockParseFuncs[0];
        for(;;)
        {
            <<try to parse an element using the current function>>
            <<if parsing succeeded, update the range and return the element>>
            ++funcCursor;
        }
    }

When we try to parse an element with the current function pointer, we first make a copy of the line range we've been asked to parse, and call the function on that copy.
This allows the parsing function we call to modify the line range as it goes, even if it doesn't end up succeeding.
The alternative would be to make such a copy in nearly every one of the parsing functions.

    <<try to parse an element using the current function>>=
    LineRange lineRange = *ioLineRange;
    MgElement* element = (*funcCursor)( context, inputFile, &lineRange );

If the parsing function succeeded (returning a non-`NULL` value), then we can go ahead and overwrite the original line range with the modified copy, before returning the element that was parsed.

    <<if parsing succeeded, update the range and return the element>>=
    if( element )
    {
        *ioLineRange = lineRange;            
        return element;
    }

Because we try the parsing function in order, we need to pay a little attention to how we arrange them in the array.

    <<block-level parsing function pointers>>=
    <<simple block-level parsing function pointers>>
    <<horizontal rule parsing function pointers>>
    <<list parsing function pointers>>
    <<setext header parsing function pointers>>
    <<default paragraph parsing function pointer>>

We start with the cases that are simple enough to identify that they can't really give "false positives."

    <<simple block-level parsing function pointers>>=
    &ParseLinkDefinition,
    &ParseTable,
    &ParseBlockLevelHtml,
    &ParseBlockQuote,
    &ParseIndentedCode,
    &ParseBracketedCode_Backtick,
    &ParseBracketedCode_Tilde,
    &ParseAtxHeader,

Next we check for horizontal rules, since some of their patterns could otherwise be matched as unordered lists.
For example, a line that is just `* * *` should be a horizontal rule, but also looks like a list item with the text `* *`.

    <<horizontal rule parsing function pointers>>=
    &ParseHorizontalRule_Hypen,
    &ParseHorizontalRule_Asterisk,
    &ParseHorizontalRule_Underscore,

    <<list parsing function pointers>>=
    &ParseOrderedList,
    &ParseUnorderedList,

We currently check for setext-style headers late in the list, since they don't pay attention to the text on their first line, and it seemed "safer" to give other rules a chance.
In retrospect, this argument doesn't seem to make much sense, and should probably be revisited.

    <<setext header parsing function pointers>>=
    &ParseSetextHeader1,
    &ParseSetextHeader2,

Finally, we parse using our default rule which creates an ordinary text paragraph.
This rule can match on any non-blank input line, so we need to check it last or else it will never let another rule match.
Luckily, this also means we don't have to worry about going through our whole list of function pointers without finding a match.

    <<default paragraph parsing function pointer>>=
    &ParseDefaultParagraph,


The following sections follow the order of presentation in John Gruber's original Markdown reference,
and typically begin with a quotation from it.

Inline HTML
-----------

The Markdown reference says:

> For any markup that is not covered by Markdown's syntax, you simply
> use HTML itself. There's no need to preface it or delimit it to
> indicate that you're switching from Markdown to HTML; you just use
> the tags.
> 
> The only restrictions are that block-level HTML elements -- e.g. `<div>`,
> `<table>`, `<pre>`, `<p>`, etc. -- must be separated from surrounding
> content by blank lines, and the start and end tags of the block should
> not be indented with tabs or spaces.

    <<block-level parsing definitions>>=
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
        if( c != '<' ) return NULL;

        int d = MgGetChar( &reader );
        if( !isalpha(d) ) return NULL;

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


Paragraphs
----------

The Markdown reference says:

> A paragraph is simply one or more consecutive lines of text, separated
> by one or more blank lines. (A blank line is any line that looks like a
> blank line -- a line containing nothing but spaces or tabs is considered
> blank.) Normal paragraphs should not be indented with spaces or tabs.

As described above, we will let other parsing functions have a chance before we try to parse a "normal" paragraph, so we don't bother with the issue of indentation.
We do, howeer, need to deal with the definition of a "blank line."

    <<global:block-level parsing utilities>>=
    MgBool IsBlankLine( MgLine* line )
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

With definition in place, we can parse a paragraph by simply reading lines until we see one that is blank.

    <<block-level parsing definitions>>=
    BLOCK_PARSE_FUNC(ParseDefaultParagraph)
    {
        MgLine* firstLine = GetLine( ioLineRange );
        MgLine* lastLine = firstLine;

        for(;;)
        {
            MgLine* line = GetLine( ioLineRange );
            if( !line || IsBlankLine(line) )
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

Note that this implementation does not handle the case of putting a list, block quote, or indented code
block right after a paragraph (without a blank line).
Input like:

    This is some code:
        int x = 9;

will be read as a single paragraph, rather than a one-line paragraph followed by an indented code block.
This is probably not a good behavior, but fixing it would probably call for a significant change to our parsing strategy.

Setext-Style Headers
--------------------

The Markdown reference says:

> Setext-style headers are "underlined" using equal signs (for first-level
> headers) and dashes (for second-level headers). For example:
> 
>     This is an H1
>     =============
> 
>     This is an H2
>     -------------
> 
> Any number of underlining `=`'s or `-`'s will work.

Since the only difference here is the character being matched, `'='` or `'-'`, we will route both of these cases through a single function, `ParseSetextHeader()`.

    <<block-level parsing declarations>>=
    MgElement* ParseSetextHeader(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange*      ioLineRange,
        char            c,
        MgElementKind   kind );

The two parsing functions are then simple wrappers around `ParseSetextHeader()`.
Each selects a difference character to use for `c`.

    <<block-level parsing definitions>>=
    BLOCK_PARSE_FUNC(ParseSetextHeader1)
    {
        return ParseSetextHeader(
            context, inputFile,
            ioLineRange,
            '=',
            kMgElementKind_Header1 );
    }

    BLOCK_PARSE_FUNC(ParseSetextHeader2)
    {
        return ParseSetextHeader(
            context, inputFile,
            ioLineRange,
            '-',
            kMgElementKind_Header2 );
    }

In order to parse a setext-style header for a given character `c`,
we first try to read two lines, then check if the second line
consists entirely of character `c`.
If we are successful, we will create the appropriate element.

    <<block-level parsing definitions>>=
    MgElement* ParseSetextHeader(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange,
        char            c,
        MgElementKind   kind )
    {
        <<try to read two lines>>

        <<check if second line is just character `c`>>

        // the inner range does not include the second line,
        // so we can't just use the Snip() function for everything
        LineRange innerRange = MgInclusiveLineRange(firstLine, firstLine);
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

Trying to read two lines involves calling `GetLine()` twice.
If the second call returns `NULL`, then we have failed.

    <<try to read two lines>>=
    MgLine* firstLine = GetLine(ioLineRange);
    MgLine* secondLine = GetLine(ioLineRange);
    if( !secondLine ) return 0;

We ignore the content of the first line, because we assume that
other parsing callbacks have been given a chance to match it before us.

Checking if the second line consists entirely of character `c` is
done with a subroutine.

    <<check if second line is just character `c`>>=
    if(!LineIsAll(secondLine, c))
        return 0;

Atx-Style Headers
-----------------

The Markdown reference says:

> Atx-style headers use 1-6 hash characters at the start of the line,
> corresponding to header levels 1-6. For example:
> 
>     # This is an H1
> 
>     ## This is an H2
> 
>     ###### This is an H6
> 
> Optionally, you may "close" atx-style headers. This is purely
> cosmetic -- you can use this if you think it looks better. The
> closing hashes don't even need to match the number of hashes
> used to open the header. (The number of opening hashes
> determines the header level.) :
> 
>     # This is an H1 #
> 
>     ## This is an H2 ##
> 
>     ### This is an H3 ######

    <<block-level parsing definitions>>=
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

Block Quotes
------------

The Markdown reference says:

> Markdown uses email-style `>` characters for blockquoting. If you're
> familiar with quoting passages of text in an email message, then you
> know how to create a blockquote in Markdown. It looks best if you hard
> wrap the text and put a `>` before every line:
> 
>     > This is a blockquote with two paragraphs. Lorem ipsum dolor sit amet,
>     > consectetuer adipiscing elit. Aliquam hendrerit mi posuere lectus.
>     > Vestibulum enim wisi, viverra nec, fringilla in, laoreet vitae, risus.
>     > 
>     > Donec sit amet nisl. Aliquam semper ipsum sit amet velit. Suspendisse
>     > id sem consectetuer libero luctus adipiscing.
> 
> Markdown allows you to be lazy and only put the `>` before the first
> line of a hard-wrapped paragraph:
> 
>     > This is a blockquote with two paragraphs. Lorem ipsum dolor sit amet,
>     consectetuer adipiscing elit. Aliquam hendrerit mi posuere lectus.
>     Vestibulum enim wisi, viverra nec, fringilla in, laoreet vitae, risus.
> 
>     > Donec sit amet nisl. Aliquam semper ipsum sit amet velit. Suspendisse
>     id sem consectetuer libero luctus adipiscing.
> 
> Blockquotes can be nested (i.e. a blockquote-in-a-blockquote) by
> adding additional levels of `>`:
> 
>     > This is the first level of quoting.
>     >
>     > > This is nested blockquote.
>     >
>     > Back to the first level.
> 
> Blockquotes can contain other Markdown elements, including headers, lists,
> and code blocks:
> 
>     > ## This is a header.
>     > 
>     > 1.   This is the first list item.
>     > 2.   This is the second list item.
>     > 
>     > Here's some example code:
>     > 
>     >     return shell_exec("echo $input | $markdown_script");


    <<block-level parsing definitions>>=
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
            while( line && !IsBlankLine(line) )
            {
                lineStart = CheckQuoteLine(line);
                if( lineStart )
                    line->text.begin = lineStart;
                lastLine = line;
                line = GetLine( ioLineRange );
            }

            // continue consuming lines until we see a non-empty line
            while( line && IsBlankLine(line) )
            {
                line = GetLine( ioLineRange );
            }
        }

        LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );

        MgElement* firstChild = ParseBlockElementsInRange(context, inputFile, innerRange);
        return MgCreateParentElement(
            kMgElementKind_BlockQuote,
            firstChild );
    }





Lists
-----

    <<block-level parsing definitions>>=
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
                if( IsBlankLine(line) )
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
            while( line && IsBlankLine(line) )
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

        MgElement* firstChild = ParseBlockElementsInRange( context, inputFile, innerRange );

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

Code Blocks
-----------

The original Markdown reference defines a syntax for code blocks based on indenting.
Many implementations also support "fenced" code blocks, so we will also allow this extension.
In additon, Mangle needs a bit of extra syntax to support introducing literate-programming scraps.

Since both kinds of code block can introduce (and reference) scraps, we route both through a single function for parsing the block contents.

    <<block-level parsing declarations>>=
    MgElement* ParseCodeBlockBody(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       inLineRange,
        char const*     langBegin,
        char const*     langEnd );

### Indented Code Blocks ###

The Markdown reference says:

> To produce a code block in Markdown, simply indent every line of the
> block by at least 4 spaces or 1 tab. For example, given this input:
> 
>     This is a normal paragraph:
> 
>         This is a code block.
> 
> ...
> 
> A code block continues until it reaches a line that is not indented
> (or the end of the article).

    <<block-level parsing definitions>>=
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

            // if the line that starts the paragraph looks like a
            // literate scrap introduction `<< foo >>=`, then end
            // this code block so we can start a new one.
            if( line != firstLine
                && CheckLiterateScrapIntroductionLine(context, inputFile, lineStart, line) )
            {
                break;
            }

            // we are starting a paragraph within the code block

            // continue consuming lines until we see an empty line
            while( line && !IsBlankLine(line) )
            {
                lineStart = CheckIndentedCodeLine(line);
                if(!lineStart)
                    break; // end of the code element...

                line->text.begin = lineStart;
                lastLine = line;
                line = GetLine( ioLineRange );
            }

            // continue consuming lines until we see a non-empty line
            while( line && IsBlankLine(line) )
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



### Fenced Code Blocks ###

    <<block-level parsing definitions>>=
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


### Literate Scraps ###

    <<block-level parsing definitions>>=
    MgBool CheckLiterateScrapIntroductionLine(
        MgContext*      context,
        MgInputFile*    inputFile,
        const char*     lineStart,
        MgLine*         line )
    {
        MgString text = MgMakeString(lineStart, line->text.end);
        MgScrapKind scrapKind = kScrapKind_Unknown;
        char const* scrapIdBegin    = 0;
        char const* scrapIdEnd      = 0;
        char const* scrapNameBegin  = 0;
        char const* scrapNameEnd    = 0;

        return ParseLiterateScrapIntroduction(
            context, inputFile, text,
            &scrapKind,
            &scrapIdBegin,      &scrapIdEnd,
            &scrapNameBegin,    &scrapNameEnd );
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
            context, inputFile, firstLine->text,
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


    <<block-level parsing definitions>>=
    MgBool ParseLiterateScrapIntroduction(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgString        text,
        MgScrapKind*    outScrapKind,
        char const**    outScrapIdBegin,
        char const**    outScrapIdEnd,
        char const**    outScrapNameBegin,
        char const**    outScrapNameEnd )
    {
        MgReader reader;
        MgInitializeStringReader( &reader, text );

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

        if( MgPeekChar(&reader) == '+' )
            MgGetChar(&reader);
        if( MgGetChar(&reader) != '=' )
            return MG_FALSE;

        SkipWhiteSpace( &reader );
        if( MgGetChar(&reader) != -1 )
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

Horizontal Rules
----------------

The Markdown reference says:

> You can produce a horizontal rule tag (`<hr />`) by placing three or
> more hyphens, asterisks, or underscores on a line by themselves. If you
> wish, you may use spaces between the hyphens or asterisks. Each of the
> following lines will produce a horizontal rule:
> 
>     * * *
> 
>     ***
> 
>     *****
> 
>     - - -
> 
>     ---------------------------------------


    <<block-level parsing definitions>>=
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
            MgMakeString(NULL, NULL) );
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

Link Definition
---------------

Links are span-level elements, but link *definitions* occur at block level.
The reference says:

> Then, anywhere in the document, you define your link label like this,
> on a line by itself:
> 
>     [id]: http://example.com/  "Optional Title Here"
> 
> That is:
> 
> *   Square brackets containing the link identifier (optionally
>     indented from the left margin using up to three spaces);
> *   followed by a colon;
> *   followed by one or more spaces (or tabs);
> *   followed by the URL for the link;
> *   optionally followed by a title attribute for the link, enclosed
>     in double or single quotes, or enclosed in parentheses.


    <<block-level parsing definitions>>=
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
            MgMakeString(NULL, NULL));
    }

Tables
------

Many implementations of Markdown also support syntax for creating tables, and we will also support this extension.


    <<block-level parsing definitions>>=
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
        MgLine* alignmentLine   = NULL;
        MgLine* firstLine       = NULL;
        MgLine* lastLine        = NULL;
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


Meta-Data
---------

    <<block-level parsing definitions>>=
    MgElement* ParseMetaData(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange*      ioLineRange )
    {
        MgLine* firstLine = GetLine( ioLineRange );
        if( !firstLine )
            return NULL;

        MgReader reader;
        InitializeLineReader( &reader, firstLine );

        // meta-data line can't start with whitespace
        if( isspace(MgPeekChar(&reader)) )
            return NULL;

        // read until a ':'
        MgString key = MgFindMatchingString(&reader, ':', 1);
        if( !key.end )
            return NULL;

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


    /*
    Like `ParseBlockElements`, but only handles meta-data elements, and
    not general Markdown document content.
    */
    MgElement* MgParseMetaDataElements(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         beginLines,
        MgLine*         endLines )
    {
        MgElement* firstElement = NULL;
        MgElement* lastElement  = NULL;

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

Utilities
---------

    <<block-level parsing utilities>>=
    void SkipEmptyLines(
        LineRange*  ioLineRange )
    {
        for(;;)
        {
            MgLine* begin = ioLineRange->begin;
            if( begin == ioLineRange->end )
                return;
            if( !IsBlankLine(begin) )
                return;
            
            ioLineRange->begin = begin + 1;
        }
    }



    <<global:block-level parsing utilities>>=
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

Some forward declarations (TODO: put these in a better place):

    <<block-level parsing declarations>>=
    char const* CheckIndentedCodeLine(
        MgLine* line );


Organization
------------

    <<global:block-level parsing>>=
    <<block-level parsing types>>
    <<block-level parsing declarations>>
    <<block-level parsing utilities>>
    <<block-level parsing definitions>>
    <<`ParseBlockElement` function>>