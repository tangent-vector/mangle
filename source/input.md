Reading Input Files
===================

    <<global:input definitions>>=
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

    void MgReadLines(
        MgContext*      context,
        MgInputFile*    inputFile )
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
    }

    void MgParseInputFileText(
        MgContext*      context,
        MgInputFile*    inputFile )
    {
        MgReadLines( context, inputFile );

        MgElement* firstElement = MgParseBlockElements(
            context,
            inputFile,
            inputFile->beginLines,
            inputFile->endLines );
        inputFile->firstElement = firstElement;
    }

    void MgParseMetaDataText(
        MgContext*      context,
        MgInputFile*    inputFile )
    {
        MgReadLines( context, inputFile );

        MgElement* firstElement = MgParseMetaDataElements(
            context,
            inputFile,
            inputFile->beginLines,
            inputFile->endLines );
        inputFile->firstElement = firstElement;
    }

    MgInputFile* MgAllocateInputFile(
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
            return NULL;
        inputFile->path         = path;
        inputFile->text         = text;
        inputFile->firstElement = 0;
        inputFile->next         = 0;
        inputFile->allocatedFileData = 0;
        inputFile->firstReferenceLink = 0;

        return inputFile;
    }

    MgInputFile* MgAddInputFileText(
        MgContext*    context,
        const char* path,
        const char* textBegin,
        const char* textEnd )
    {
        MgInputFile* inputFile = MgAllocateInputFile(
            context,
            path,
            textBegin,
            textEnd );
        if( !inputFile )
            return NULL;

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

    char* MgReadFileStreamContent(
        MgContext*  context,
        char const* path,
        FILE*       stream,
        int*        outSize )
    {
        int begin = ftell(stream);
        fseek(stream, 0, SEEK_END);
        int end = ftell(stream);
        fseek(stream, begin, SEEK_SET);
        int size = end - begin;

        // allocate buffer for input file
        char* fileData = (char*) malloc(size + 1);
        if( !fileData )
        {
            fprintf(stderr, "failed to allocate buffer for \"%s\"\n", path);        
            return NULL;
        }
        // we NULL-terminate the buffer just in case
        // (but the code should never rely on this)
        fileData[size] = 0;

        int sizeRead = fread(fileData, 1, size, stream);
        if( sizeRead != size )
        {
            fprintf(stderr, "failed to read from \"%s\"\n", path);
            free(fileData);
            return NULL;
        }

        *outSize = size;
        return fileData;
    }

    MgInputFile* MgAddInputFileStream(
        MgContext*  context,
        char const* path,
        FILE*       stream )
    {
        if( !context )  return 0;
        if( !stream )   return 0;

        int size = 0;
        char* fileData = MgReadFileStreamContent( context, path, stream, &size );
        if( !fileData )
            return NULL;

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
        FILE* stream = NULL;
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

    MgInputFile* MgAddMetaDataText(
        MgContext*  context,
        const char* path,
        const char* textBegin,
        const char* textEnd )
    {
        // don't allow multiple meta-data files
        if( context->metaDataFile )
            return NULL;

        MgInputFile* inputFile = MgAllocateInputFile(
            context,
            path,
            textBegin,
            textEnd );
        if( !inputFile )
            return NULL;

        context->metaDataFile = inputFile;

        MgParseMetaDataText(
            context,
            inputFile );
        return inputFile;
    }

    MgInputFile* MgAddMetaDataFileStream(
        MgContext*  context,
        const char* path,
        FILE*       stream )
    {
        if( !context )  return 0;
        if( !stream )   return 0;

        int size = 0;
        char* fileData = MgReadFileStreamContent( context, path, stream, &size );
        if( !fileData )
            return NULL;

        MgString text = { fileData, fileData + size };
        MgInputFile* inputFile = MgAddMetaDataText(
            context,
            path,
            fileData,
            fileData + size );
        inputFile->allocatedFileData = fileData;
        return inputFile;
    }


    MgInputFile* MgAddMetaDataFile(
        MgContext*  context,
        const char* path )
    {
        FILE* stream = NULL;
        if( !context )  return 0;
        if( !path )     return 0;

        stream = fopen(path, "rb");
        if( !stream )
        {
            fprintf(stderr, "mangle: failed to open \"%s\" for reading\n", path);
            return 0;
        }

        MgInputFile* inputFile = MgAddMetaDataFileStream(
            context,
            path,
            stream );
        fclose(stream);
        return inputFile;
    }
