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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "markdown.h"

//

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

int main(
    int     argc,
    char**  argv )
{
    Options options;
    InitializeOptions( &options );

    if( !ParseOptions( &options, &argc, argv ) )
    {

        fprintf(stderr, "usage: %s file1.md [...]", argv[0]);
        exit(1);
    }

    if( !argc )
    {
        fprintf(stderr, "no input files\n");
        exit(0);
    }

    int status = 0;

    // read all of the input files into the context
    MgContext context;
    MgInitialize( &context );

    //
    if( options.metaDataFilePath )
    {
        MgAddMetaDataFile( &context, options.metaDataFilePath );
    }

    for( int ii = 0; ii < argc; ++ii )
    {
        if( !MgAddInputFilePath( &context, argv[ii] ) )
        {
            status = 1;
            continue;
        }
    }

    MgWriteAllCodeFiles( &context );

    MgWriteAllDocFiles( &context );

    MgFinalize( &context );

    return status;
}

