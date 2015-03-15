Parsing
=======

    <<global:parsing definitions>>=
    enum
    {
        kMaxHeaderLevel = 6,
    };

    /*
    Flags for parsing span-level elements in Markdown source.
    */
    enum
    {
        /* No flags. */
        kMgSpanFlags_None = 0x00,

        /* Enable escaping of HTML entiteis like `&amp;` into characters like `&`.*/
        kMgSpanFlag_EscapeHtmlEntities = 0x01,

        /* Disable processing of standard Markdown syntax. */
        kMgSpanFlag_DontProcessMarkdown = 0x02,

        /* Default behavior: escape HTML entities, process Markdown. */
        kMgSpanFlags_Default =
            kMgSpanFlag_EscapeHtmlEntities,

        /* Inside a code block: escape HTML, but don't process Markdown */
        kMgSpanFlags_CodeBlock =
            kMgSpanFlag_EscapeHtmlEntities
            | kMgSpanFlag_DontProcessMarkdown,

        /* Inline code: handle the same as a code block */
        kMgSpanFlags_InlineCode = kMgSpanFlags_CodeBlock,

        /* Inside an HTML block: don't process Markdown. */
        kMgSpanFlags_HtmlBlock =
            kMgSpanFlag_DontProcessMarkdown,
    };
    typedef unsigned MgSpanFlags;

    /*
    When encountering either a reference to or a definition of a "reference-style"
    link in the document, call this function to get or create the object to represent
    the link. At a definition site, fill in the `MgReferenceLink*` that was returned,
    to provide information that will be used at any sites that reference it.
    */
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

    /*
    Allocate and add a new attribute to an existing element, with the
    specified NULL-terminated `id` and value.
    */
    MgAttribute* MgAddAttribute(
        MgElement*  element,
        char const* id,
        MgString      val )
    {
        MgAttribute* attr = (MgAttribute*) malloc(sizeof(MgAttribute));
        attr->next  = NULL;
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

    /*
    Allocate an add a new atttribute to an existing element, with the
    specified NULL-terminated `id`, but with no value. The caller is
    expected to fill in of the fields in the `union` inside the
    attribute, based on the chosen ID.
    */
    MgAttribute* MgAddCustomAttribute(
        MgElement*  element,
        char const* id)
    {
        return MgAddAttribute(element, id, MgMakeString(NULL, NULL));
    }

    MgElement* MgCreateElementImpl(
        MgElementKind   kind,
        MgString          text,
        MgElement*      firstChild )
    {
        MgElement* element = (MgElement*) malloc(sizeof(MgElement));
        element->kind       = kind;
        element->text       = text;
        element->firstAttr  = NULL;
        element->firstChild = firstChild;
        element->next       = NULL;
        return element;
    }

    /*
    Create a leaf document element, with the specified kind and text.
    */
    MgElement* MgCreateLeafElement(
        MgElementKind   kind,
        MgString          text )
    {
        return MgCreateElementImpl(
            kind,
            text,
            NULL );  // no children
    }

    /*
    Create a parent document element, with the specified first child in
    the linked list of child elements.
    */
    MgElement* MgCreateParentElement(
        MgElementKind   kind,
        MgElement*      firstChild )
    {
        return MgCreateElementImpl(
            kind,
            MgMakeString(NULL, NULL), // no text
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

            fileGroup = fileGroup->next;
        }

        return 0;
    }

    /*
    When encountering either a reference to or a definition of a scrap,
    call this function to get or create the object that represents the scrap
    group with that `id` for the given `file`.

    The `kind` can either be `kMgScrapKind_Unknown` if you don't care what
    kind of scrap it is, or a specific scrap kind if you want to set the
    scrap kind as part of retrieving it. (TODO: separate those steps)
    */
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

    /*
    Add a particular scrap definition to a group of scraps with the same
    ID in the same file.
    */
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

    /*
    Look ahead in the reader for an occurence of character `c`, `count` times
    in a row. Return a pointer to right before the first character in the match,
    and leave the cursor of the reader pointing right after the match.

    If no match is found, returns NULL.
    */
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

    /*
    Similar to `MgFindMatching`, except that it returns the intial cursor of the
    reader as the `begin` of the result string, and the result from
    `MgFindMatching` as the `end`.

    Note: this means that when no match is found, the `end` of the result will
    be NULL, but the `begin` won't be.
    */
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

    /*
    Return line number and column information (1-based) for a location in the
    given input file, represented by the given pointer into the given line.
    */
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
