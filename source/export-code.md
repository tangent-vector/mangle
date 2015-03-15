Code Export
===========

    <<global:code export definitions>>=
    void ExportScrapFileGroup(
        MgContext*        context,
        MgScrapFileGroup* scrapFileGroup,
        MgWriter*         writer );

    void ExportScrapElements(
        MgContext*  context,
        MgScrap*    scrap,
        MgElement*  firstElement,
        MgWriter*   writer,
        int         indent );

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

    static void Indent(
        MgWriter*   writer,
        int         indent )
    {
        for( int ii = 1; ii < indent; ++ii )
            MgWriteCString(writer, " ");    
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

        Indent( writer, loc.col );
    }

    void ExportScrapElement(
        MgContext*  context,
        MgScrap*    scrap,
        MgElement*  element,
        MgWriter*   writer,
        int         indent )
    {
        switch( element->kind )
        {
        case kMgElementKind_CodeBlock:
        case kMgElementKind_Text:
            MgWriteString(writer, element->text);
            ExportScrapElements(context, scrap, element->firstChild, writer, indent);
            break;

        case kMgElementKind_NewLine:
            MgWriteString(writer, element->text);
            Indent( writer, indent );
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
        MgScrap*    scrap,
        MgElement*  firstElement,
        MgWriter*   writer,
        int         indent )
    {
        for( MgElement* element = firstElement; element; element = element->next )
            ExportScrapElement( context, scrap, element, writer, indent );
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
            writer,
            scrap->sourceLoc.col );
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
