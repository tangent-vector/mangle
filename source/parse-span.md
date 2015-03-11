Parsing Span-Level Elements
===========================

    <<global:span-level parsing definitions>>=
    /*
    ### Span-Level Elements ###
    */
    typedef struct SpanWriterT
    {
        MgElement* firstElement;
        MgElement* lastElement;

        char const* spanStart;
        char const* spanEnd;
    } SpanWriter;

    void InitializeSpanWriter(
        SpanWriter* writer )
    {
        writer->firstElement = 0;
        writer->lastElement = 0;
        writer->spanStart = 0;
        writer->spanEnd = 0;
    }

    void BeginSpan(
        SpanWriter* writer,
        char const* spanStart )
    {
        writer->spanStart = spanStart;
        writer->spanEnd = spanStart;
    }

    void ExtendSpan(
        SpanWriter* writer,
        char const* spanEnd )
    {
        writer->spanEnd = spanEnd;
    }

    void AddSpanElementImpl(
        SpanWriter* writer,
        MgElement*  element )
    {
        // add the element to the list
        if( writer->lastElement )
        {
            writer->lastElement->next = element;
        }
        else
        {
            writer->firstElement = element;
        }
        writer->lastElement = element;

        writer->spanStart = 0;
        writer->spanEnd = 0;    
    }

    void FlushSpan(
        SpanWriter* writer )
    {
        MgElement* element = MG_NULL;
        if( writer->spanStart == writer->spanEnd )
            return;

        element = MgCreateLeafElement(
            kMgElementKind_Text,
            MgMakeString(writer->spanStart, writer->spanEnd) );

        AddSpanElementImpl( writer, element );
    }

    void AddSpanElement(
        SpanWriter* writer,
        MgElement*  element )
    {
        FlushSpan( writer );
        AddSpanElementImpl( writer, element );
    }


    #include <memory.h>

    void AddLiteralSpan(
        SpanWriter* writer,
        char const* text )
    {
        FlushSpan( writer );
        BeginSpan( writer, text );
        ExtendSpan( writer, text + strlen(text) );
        FlushSpan( writer );
    }

    void ReadLineSpans(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        char const*     textBegin,
        char const*     textEnd,
        MgSpanFlags       flags,
        SpanWriter*     writer );

    /*
    Read span-level elements from the range of text given by
    `textBegin` and `textEnd`, using the given flags. The range
    of characters given should belong to `line` in `inputFile`,
    which should be loaded in `context`.
    */
    MgElement* MgReadSpanElements(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgString        text,
        MgSpanFlags     flags )
    {
        SpanWriter writer;
        InitializeSpanWriter( &writer );
        ReadLineSpans(context, inputFile, line, text.begin, text.end, flags, &writer);
        return writer.firstElement;
    }

    //
    //
    //

    MgElement* ParseHtmlEntity(
        MgContext*        context,
        MgInputFile*      inputFile,
        MgReader*   reader,
        MgSpanFlags       flags,
        char            c,
        MgElementKind   kind )
    {
        if( !(flags & kMgSpanFlag_EscapeHtmlEntities) )
            return 0;

        int d = MgGetChar( reader );
        if( d != c )
            return 0;

        return MgCreateParentElement(
            kind, 0 );
    }

    MgElement* ParseHtmlEntity_LessThan(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        return ParseHtmlEntity(context, inputFile, reader, flags, '<', kMgElementKind_LessThanEntity);
    }

    MgElement* ParseHtmlEntity_GreaterThan(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        return ParseHtmlEntity(context, inputFile, reader, flags, '>', kMgElementKind_GreaterThanEntity);
    }

    MgElement* ParseHtmlEntity_Ampersand(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        return ParseHtmlEntity(context, inputFile, reader, flags, '&', kMgElementKind_AmpersandEntity);
    }

    MgElement* ParseScrapRef(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        MgString scrapID;

        if( MgGetChar(reader) != '<' ) return 0;
        if( MgGetChar(reader) != '<' ) return 0;

        char const* idBegin = reader->cursor;
        char const* idEnd   = idBegin;
        for(;;)
        {
            idEnd = reader->cursor;
            int c = MgGetChar( reader );
            if( c == -1 )
                return 0;

            if( c == '>' )
            {
                int d = MgGetChar( reader );
                if( d == '>' )
                {
                    break;
                }
            }
        }

        // In order to avoid accidentally treating an expression
        // with both left and right shifts as a scrap reference,
        // we require that there be no whitespace after the `<<`.
        // This lets a user easily avoid problems by inserting
        // spaces into their expressions:
        //
        // TODO: the " &nbsp; " in the next line should really be
        // blank, to illustrate the point, but right now Mangle
        // assumes it is a scrap reference (exactly as described
        // by this logic...)
        // scrap ref: `1 << &nbsp; foo &nbsp; >> bar`
        // shifts:      `1 << foo >> bar`

        if( idBegin != idEnd && isspace(*idBegin) )
            return 0;

        scrapID = MgMakeString( idBegin, idEnd );
        MgScrapFileGroup* scrapFileGroup = MgFindOrCreateScrapGroup(
            context,
            kScrapKind_Unknown,
            scrapID,
            inputFile );

        MgElement* element = MgCreateParentElement(
            kMgElementKind_ScrapRef,
            0 );
        MgAttribute* attr = MgAddCustomAttribute(element, "$scrap-group");
        attr->scrapFileGroup = scrapFileGroup;
        attr = MgAddCustomAttribute(element, "$resume-at");
        MgSourceLoc sourceLoc = MgGetSourceLoc( inputFile, line, reader->cursor );
        attr->sourceLoc = sourceLoc;

        return element;
    }

    MgElement* ParseEm(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags,
        char            c )
    {
        MgElement* inner = MG_NULL;
        if( flags & kMgSpanFlag_DontProcessMarkdown )
            return MG_NULL;

        // follow GitHub Flavored Markdown, in only
        // allowing underscores for <em> when
        // they mark a whole word...
        if( (c == '_') && !isspace(MgGetPrecedingChar(reader)) )
            return MG_NULL;

        int count = 0;
        for(; count < 2; ++count)
        {
            int d = MgGetChar( reader );
            if( d != c )
            {
                MgUnGetChar( reader, d );
                break;
            }
        }
        if( !count )
            return MG_NULL;

        int e = MgPeekChar( reader );
        if( isspace(e) )
            return MG_NULL; // can't start with white-space

        // appears to be the start of a span.
        // now we need to find the matching marker(s)
        char const* start = reader->cursor;
        char const* end = MgFindMatching( reader, c, count );
        if( !end )
            return MG_NULL;

        if( (c == '_') && isalpha(MgPeekChar(reader)) )
            return MG_NULL;

        // need to scan the inner text for other span markup
        inner = MgReadSpanElements( context, inputFile, line, MgMakeString(start, end), flags );

        return MgCreateParentElement(
            count == 2 ? kMgElementKind_Strong : kMgElementKind_Em ,
            inner );
    }

    MgElement* ParseEm_Underscore(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        return ParseEm(
            context,
            inputFile,
            line,
            reader,
            flags,
            '_' );
    }

    MgElement* ParseEm_Asterisk(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        return ParseEm(
            context,
            inputFile,
            line,
            reader,
            flags,
            '*' );
    }

    MgElement* ParseInlineCode(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        MgElement* inner = MG_NULL;
        if( flags & kMgSpanFlag_DontProcessMarkdown )
            return MG_NULL;

        int count = 0;
        for(; count < 2; ++count)
        {
            int d = MgGetChar( reader );
            if( d != '`' )
            {
                MgUnGetChar( reader, d );
                break;
            }
        }
        if( !count )
            return MG_NULL;

        // allow an optional space at start,
        // which will get trimmed
        int lead = MgGetChar(reader);
        if( lead != ' ' )
        {
            MgUnGetChar(reader, lead);
        }

        // appears to be the start of a span.
        // now we need to find the matching marker
        char const* start = reader->cursor;
        char const* end = MgFindMatching( reader, '`', count );
        if( !end )
            return MG_NULL;

        // allow an optional space at end
        // which will get trimmed
        if( start != end && (*(end-1) == ' ') )
            --end;

        inner = MgReadSpanElements( context, inputFile, line, MgMakeString(start, end), kMgSpanFlags_InlineCode );

        return MgCreateParentElement(
            kMgElementKind_InlineCode,
            inner );
    }

    MgElement* ParseLink(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        MgString text;
        if( flags & kMgSpanFlag_DontProcessMarkdown )
            return 0;

        int textOpenBrace = MgGetChar( reader );
        if( textOpenBrace != '[' )
            return 0;

        text = MgFindMatchingString( reader, ']', 1 );
        if( !text.end )
            return 0;

        int targetOpenBrace = MgGetChar( reader );
        if( targetOpenBrace == '(' )
        {
            MgElement* inner    = MG_NULL;
            MgElement* link     = MG_NULL;

            // inline link
            char const* targetBegin = reader->cursor;
            char const* targetEnd = MgFindMatching( reader, ')', 1 );
            if( !targetEnd )
                return 0;

            inner = MgReadSpanElements( context, inputFile, line, text, flags );

            link = MgCreateParentElement(
                kMgElementKind_Link,
                inner );

            MgAddAttribute(link, "href", MgMakeString(targetBegin, targetEnd));
            return link;
        }
        else if( targetOpenBrace == '[' )
        {
            // reference link
            MgString id = MgFindMatchingString(reader, ']', 1);
            if( !id.end )
                return 0;

            if( MgIsEmptyString(id) )
            {
                id = text;
            }

            // \todo: need to save this identifier,
            // so taht we can look up the link target later...
            MgReferenceLink* referenceLink = MgFindOrCreateReferenceLink(
                inputFile,
                id );

            MgElement* inner = MgReadSpanElements( context, inputFile, line, text, flags );

            MgElement* link = MgCreateParentElement(
                kMgElementKind_ReferenceLink,
                inner );

            MgAttribute* attr = MgAddCustomAttribute(link, "$referenceLink");
            attr->referenceLink = referenceLink;

            return link;
        }

        return 0;
    }

    //

    MgElement* TryParseSpanElement(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgSpanFlags       flags )
    {
        typedef MgElement* (*ParseSpanFunc)( MgContext*, MgInputFile*, MgLine*, MgReader*, MgSpanFlags );
        static const ParseSpanFunc parseSpanFuncs[] =
        {
            &ParseScrapRef,
            &ParseLink,
            &ParseHtmlEntity_LessThan,
            &ParseHtmlEntity_GreaterThan,
            &ParseHtmlEntity_Ampersand,
            &ParseEm_Underscore,
            &ParseEm_Asterisk,
            &ParseInlineCode,
            0,
        };
        ParseSpanFunc const* parseFunc = &parseSpanFuncs[0];
        do
        {
            MgReader tempReader = *reader;
            MgElement* p = (*parseFunc)( context, inputFile, line, &tempReader, flags );
            if( p )
            {
                reader->cursor = tempReader.cursor;
                return p;              
            }
            ++parseFunc;
        }
        while(*parseFunc);

        return 0;
    }

    //

    void ReadLineSpans(
        MgContext*    context,
        MgInputFile*      inputFile,
        MgLine*       line,
        char const* textBegin,
        char const* textEnd,
        MgSpanFlags   flags,
        SpanWriter* writer )
    {
        MgString string = { textBegin, textEnd };
        MgReader reader;
        MgInitializeStringReader( &reader, string );

        BeginSpan( writer, reader.cursor );

        while(!MgAtEnd(&reader))
        {
            // look for a match among our various cases
            MgElement* element = TryParseSpanElement( context, inputFile, line, &reader, flags );
            if( element )
            {
                AddSpanElement( writer, element );
                BeginSpan( writer, reader.cursor );
                continue;
            }

            // fallback position - read one character
            // and add it to our current textual span
            int c = MgGetChar( &reader );

            // okay, with one special case for the '\'
            // escape character...
            //
            // \todo: where do escapes get ignored?
            if( c == '\\'
                && !(flags & kMgSpanFlag_DontProcessMarkdown))
            {
                // end the current span, since we need
                // to skip the '\'
                FlushSpan( writer );

                // start fresh span *after* the backslash
                BeginSpan( writer, reader.cursor );

                // read the escaped character, so that
                // it won't get a chance to be processed
                // by the other rules
                c = MgGetChar( &reader );
            }

            // default: just extend the span
            ExtendSpan( writer, reader.cursor );
        }
        FlushSpan( writer );
    }

    /*
    Read span-level elements from the range of lines given by
    `beginLines` and `endLines`, using the given flags. The range
    of lines given should belong to `inputFile`, which should be
    loaded in `context`.
    */
    MgElement* MgReadSpanElementsFromLines(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         beginLines,
        MgLine*         endLines,
        MgSpanFlags       flags )
    {
        SpanWriter writer;
        InitializeSpanWriter( &writer );

        for( MgLine* line = beginLines; line != endLines; ++line )
        {
            ReadLineSpans( context, inputFile, line, line->text.begin, line->text.end, flags, &writer );
            MgElement* newLine = MgCreateLeafElement(
                kMgElementKind_NewLine,
                MgTerminatedString("\n"));
            AddSpanElement( &writer, newLine );
        }
        return writer.firstElement;
    }
