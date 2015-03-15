Command-Line Options
====================


    <<global:options definitions>>=
    /* Command-Line Options */

    typedef struct OptionsT
    {
        char const* executableName;
        char const* docOutputPath;
        char const* sourceOutputPath;
        char const* metaDataFilePath;
    } Options;

    void InitializeOptions(
        Options*    options )
    {
        options->executableName     = "mangle";
        options->docOutputPath      = 0;
        options->sourceOutputPath   = 0;
        options->metaDataFilePath   = 0;
    }

    int ParseOptions(
        Options*    options,
        int*        ioArgCount,
        char**      argv )
    {
        int     remaining   = *ioArgCount;
        char**  readCursor  = argv;

        char**  writeCursor = argv;
        int     outArgCount = 0;

        if( remaining > 0 )
        {
            options->executableName = *readCursor++;
            --remaining;
        }

        while(remaining)
        {
            char* option = *readCursor++;
            --remaining;

            if( option[0] == '-' )
            {
                if( option[1] == '-' )
                {
                    if( option[2] == 0 )
                    {
                        // treat an `--` option as marking the end of the list
                        break;
                    }
                    else if( strcmp(option+2, "meta") == 0 )
                    {
                        // meta-data file
                        if( remaining != 0 )
                        {
                            options->metaDataFilePath = *readCursor++;
                            --remaining;
                            continue;
                        }
                        else
                        {
                            fprintf(stderr, "expected argument for option %s\n", option);
                            return 0;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "unknown option: %s\n", option);
                        return 0;
                    }
                }
                else
                {
                    // If we wanted to allow "short" options, this is where
                    // they would be processed.

                    fprintf(stderr, "unknown option: %s\n", option);
                    return 0;
                }
            }

            // default logic
            *writeCursor++ = option;
            ++outArgCount;
        }

        // pass through any options after `--` without inspecting them
        while( remaining )
        {
            char* option = *readCursor++;
            --remaining;
            *writeCursor++ = option;
            ++outArgCount;
        }

        *ioArgCount = outArgCount;
        return 1;
    }
