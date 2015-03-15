HTML Export
===========

    <<global:HTML export definitions>>=
    void WriteElement(
        MgContext*    context,
        MgElement*  pp,
        MgWriter*     output );

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
        case kMgElementKind_NewLine:
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
        case kMgElementKind_NewLine:
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

    //

    static void WriteToFile(
        FILE* file,
        char const* begin,
        char const* end)
    {
        fwrite(begin, 1, end-begin, file);
    }

    static void MgWriteElementText(
        MgElement*  element,
        MgWriter*   writer )
    {
        // TODO: some elements need special handling here...
        MgWriteString( writer, element->text );

        MgElement* child = element->firstChild;
        while( child )
        {
            MgWriteElementText( child, writer );
            child = child->next;
        }
    }

    MgElement* MgFindMetaDataInFile(
        MgInputFile*    file,
        const char*     key )
    {
        MgString keyString = MgTerminatedString(key);

        MgElement* element = file->firstElement;
        for(; element; element = element->next)
        {
            if( element->kind != kMgElementKind_MetaData )
                continue;

            MgAttribute* keyAttr = MgFindAttribute(element, "$key");
            if( !keyAttr )
                continue;

            if( !MgStringsAreEqualNoCase(keyString, keyAttr->val) )
                continue;

            return element;
        }

        return NULL;
    }



    MgElement* MgFindMetaData(
        MgContext*      context,
        MgInputFile*    inputFile,
        const char*     key )
    {
        MgElement* element = NULL;

        // look for file-specific meta-data
        element = MgFindMetaDataInFile( inputFile, key );
        if( element ) return element;

        // look for generic meta-data
        if( context->metaDataFile )
        {
            element = MgFindMetaDataInFile( context->metaDataFile, key );
            if( element ) return element;        
        }

        return NULL;
    }

    typedef void (*MgMetaDataFunc)(
        MgElement*  metaData,
        void*       userData );

    void MgForEachMetaDataInFile(
        MgInputFile*    file,
        const char*     key,
        MgMetaDataFunc  func,
        void*           userData )
    {
        MgString keyString = MgTerminatedString(key);

        MgElement* element = file->firstElement;
        for(; element; element = element->next)
        {
            if( element->kind != kMgElementKind_MetaData )
                continue;

            MgAttribute* keyAttr = MgFindAttribute(element, "$key");
            if( !keyAttr )
                continue;

            if( !MgStringsAreEqualNoCase(keyString, keyAttr->val) )
                continue;

            func( element, userData );
        }
    }

    void MgForEachMetaData(
        MgContext*      context,
        MgInputFile*    file,
        const char*     key,
        MgMetaDataFunc  func,
        void*           userData )
    {
        // generic meta-data first
        if( context->metaDataFile )
            MgForEachMetaDataInFile( context->metaDataFile, key, func, userData );

        // then file-specific meta-data
        MgForEachMetaDataInFile( file, key, func, userData );
    }

    void MgCssMetaDataCallback(
        MgElement*  cssElement,
        void*       userData )
    {
        MgWriter* writer = (MgWriter*) userData;
        MgWriteCString(writer, "<link rel='stylesheet' type='text/css' href='");
        MgWriteElementText(cssElement, writer);
        MgWriteCString(writer, "'>\n");    
    }

    static MgElement* MgFindTitleElement(
        MgElement* firstElement )
    {
        // only look along the top-level "spine" of the document
        MgElement* element = firstElement;
        MgElement* bestElement = NULL;
        for(; element; element = element->next)
        {
            switch(element->kind)
            {
            case kMgElementKind_Header1:
            case kMgElementKind_Header2:
            case kMgElementKind_Header3:
            case kMgElementKind_Header4:
            case kMgElementKind_Header5:
            case kMgElementKind_Header6:
                break;

            default:
                continue; // only consider headers
            }

            // here we rely on the fact that the header element
            // kinds are defined to have ascending order in the enum...
            if( !bestElement || bestElement->kind > element->kind )
            {
                bestElement = element;
            }
        }

        return bestElement;
    }

    void MgWriteDoc(
        MgContext*        context,
        MgInputFile*      inputFile,
        MgWriter*         writer )
    {
        MgWriteCString(writer,
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "    <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n");

        // try to find a title to output
        // TODO: support title coming from command line or config file
        MgElement* titleElement = MgFindMetaData(context, inputFile, "title");
        if( !titleElement )
            titleElement = MgFindTitleElement(inputFile->firstElement);
        MgWriteCString(writer, "<title>");
        if( titleElement )
            MgWriteElementText(titleElement, writer);
        MgWriteCString(writer, "</title>\n");

        // handle CSS meta-data
        MgForEachMetaData(context, inputFile, "css", &MgCssMetaDataCallback, writer);

        // TODO: add other kinds of meta-data support

        MgWriteCString(writer,
            "</head>\n"
            "<body>\n");

        WriteElements(
            context,
            inputFile->firstElement,
            writer );

        MgWriteCString(writer,
            "</body>\n");
    }

    #ifdef _MSC_VER
    char* stpcpy(char* dest, char const* src)
    {
        while( *dest++ = *src++ )
        {}
        return dest-1;
    }
    #endif

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
