The Main Entry Point
====================

The overall flow of the program is to perform initialization, parse command-line options, read input files, write output files, and finally clean up.

    <<`main` function>>=
    int main(
        int     argc,
        char**  argv )
    {
        <<initialize>>
        <<parse options>>
        <<read inputs>>
        <<write outputs>>
        return 0;
    }

Initialization
--------------

The `Context` type is defined so that it is valid when zero-initialized.

    <<initialize>>=
    MgContext context;
    memset(&context, 0, sizeof(context));

Note that there is no corresponding clean-up or "finalization" work at the end of `main`.
Since the program is about to exit anyway, releasing all the memory we've allocated would only take more code/time and not actually provide any benefit.

Parsing Command-Line Options
----------------------------

    <<parse options>>=
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

Reading Input
-------------

To read the input, we first read a meta-data file, if needed, and then any ordinary input files.

    <<read inputs>>=
    <<read meta data file, if needed>>
    <<read ordinary input files>>

We only read a meta-data file if the user has specified one on the command line.

    <<read meta data file, if needed>>=
    if( options.metaDataFilePath )
    {
        MgAddMetaDataFile( &context, options.metaDataFilePath );
    }

We read input files by looping over the argument array.
The options-parsing code will have updated `argc` and `argv` to filter out everything other than input files.

    <<read ordinary input files>>=
    for( int ii = 0; ii < argc; ++ii )
    {
        char const* path = argv[ii];
        <<read one input file from `path`>>
    }

If we encounter an error while reading an input file, we exit immediately.

    <<read one input file from `path`>>=
    if( !MgAddInputFilePath( &context, path ) )
    {
        exit(1);
    }

Writing Output
--------------

To write the output, we first write out any code files, and then any documentation files.

    <<write outputs>>=
    <<write output code files>>
    <<write output documentation files>>

### Documentation ###

In order to output documentation, we simply loop over all of the input files attached to the context, and write one HTML document for each.

    <<write output documentation files>>=
    for( MgInputFile* file = context.firstInputFile; file; file = file->next )
    {
        MgWriteDocFile( &context, file );
    }

### Code ###

In order to write the output code, we loop over all the scrap groups that were found during parsing, outputing only those with the `file:` kind.

    <<write output code files>>=
    for( MgScrapNameGroup* group = context.firstScrapNameGroup; group; group = group->next )
    {
        if( group->kind != kScrapKind_OutputFile )
            continue;

        MgWriteCodeFile( &context, group );
    }

Packaging
---------

In order to make Mangle easy to deploy, all of its code will be placed in a single `.c` file.

    <<file:mangle.c>>=
    <<license>>
    <<includes>>
    <<declarations>>
    <<definitions>>
    <<globals>>
    <<`main` function>>

### Includes ###

    <<includes>>=
    #include <assert.h>
    #include <ctype.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

### Declarations ###

    <<declarations>>=
    <<string declarations>>
    <<document declarations>>

### Definitions ###

    <<definitions>>=
    <<reader definitions>>
    <<string definitions>>
    <<parsing definitions>>
    <<span-level parsing definitions>>
    <<block-level parsing definitions>>
    <<writer definitions>>
    <<export definitions>>
    <<code export definitions>>
    <<HTML export definitions>>
    <<input definitions>>
    <<options definitions>>


Junk
----

    /*
    ## Meta-Data ##

    The user can also specify a "meta-data" file that contains declarations that
    will apply to all input files. This can be used to specify things that will
    go into headers of the HTML output documents. The functions below return
    the meta-data content in an `MgInputFile` (this is convenient since we might
    allow meta-data declarations in the ordinary files too), but the file will
    not be added to the linked list of input documents.
    */

    /*
    ## Output ##

    As input files are read, Mangle builds up a representation
    of Markdown document structure, and also creates a cross-
    referenced database of literate programming "scraps" that
    are defined or referenced in each file. Scraps with the
    same identifier are grouped together, and some of these
    groups represent source files that should be compiled
    to create a program.
    */
    /*
    Once you have the pointer to the input file or code file, you can
    use one of the following functions to output it to a file.

    The first pair of functions outputs docs/code to a file where
    the path is chosen by Mangle. For code files this will be
    the code path prefix (if any) and the name given in the literate
    code. For doc files, the name will be the doc path prefix (if any)
    and the path of the input file with any file extension replaced
    with `.html`.

    By default, Mangle will first output the content to memory,
    and then check whether that data differs from what is on
    disk already (by reading the output file). This is a
    safeguard so that we don't end up "touching" files that
    haven't really changed, causing build systems that look
    at modification times to get confused.
    */

