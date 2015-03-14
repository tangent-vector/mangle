Mangle: A Literate Programming Tool
===================================

Mangle is a tool for literate programming using Markdown
together with C-like languages. Please see the README.md
file if you want to know how to use Mangle as a command-
line tool, and to understand the kind of syntax it expects.
This header file will only document the library interface
to Mangle's internals, for the benefit of anybody who
wants to build on or improve it.


Utilities
---------

First we have common stuff shared across multiple code files.

We define our own Boolean type, since we are using C and can't assume C11.

    <<utility declarations>>+=
    typedef int MgBool;
    #define MG_TRUE     (1)
    #define MG_FALSE    (0)

We define our own `NULL`, in case we ever want to compile this code without
including `<stdlib.h>`.

    <<utility declarations>>+=
    #define MG_NULL     (0)

We use the `MgLine` type for lines that have been loaded into memory.

    <<utility declarations>>+=
    struct MgLineT
    {
        MgString      text;
        char const* originalBegin;
    };

Initially, `text.begin` and `text.end` point at the start of the line, and
just before the line-ending terminator (if any). The `originalBegin` field
always points at the very start of the line as stored on disk.

During parsing, routines that recognize a pattern might "trim" characters
from the beginning (or end, potentially) of the line so that further passes
see only a subset of characters. For example, when parsing a block quote,
with a prefix of `"> "`, we advance the `begin` pointer by two characters
before recursively parsing the quoted text.

This design lets us avoid making a lot of copies of data during parsing,
so that we can instead just use the original buffer of the file contents.



Here we go:

    <<file:mangle.c>>=
    <<license>>

    #include <assert.h>
    #include <ctype.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    /* API */

    /*
    
    ## Context ##

    All of the state for operations you do with Mangle gets
    stored in a `MgContext`. The contents of this type
    are laid bare at later in this file, but you shouldn't
    need to rely on those details as a client of the API.
    */
    typedef struct MgContextT MgContext;

    /*
    Before using other Mangle functions, you must initialize
    the context with `MgInitialize`, and when you are
    done you ought to call `MgFinalize` to let the
    library clean up any memory it has allocated.
    */

    void MgInitialize(
        MgContext*  context );

    void MgFinalize(
        MgContext*  context);

    /*
    ## Input Files ##

    After initializing a context, you then need to read in
    one or more files containing literate program code.
    The type `MgInputFile` represents an input file,
    which will ultimately be in a one-to-one relationship
    with output documentation files.
    */

    typedef struct MgInputFileT MgInputFile;

    /*
    The fundamental entry point for adding input files to
    the context is `MgAddInputFileText`. You can use
    this function directly, or one of the convenience
    routines that follows:
    */

    MgInputFile* MgAddInputFileText(
        MgContext*  context,
        const char* path,       /* path to use when picking output file name/path */
        const char* textBegin,  /* pointer to start of text buffer */
        const char* textEnd );  /* pointer to end of text buffer, or NULL if buffer is NULL-terminated */

    /*
    In the common case, the input might be files on disk, so
    you can just use `MgAddInputFileStream` or
    `MgAddInputFilePath` instead.
    */

    MgInputFile* MgAddInputFileStream(
        MgContext*  context,
        const char* path,
        FILE*       stream );

    MgInputFile* MgAddInputFilePath(
        MgContext*  context,
        const char* path );

    /*
    ## Meta-Data ##

    The user can also specify a "meta-data" file that contains declarations that
    will apply to all input files. This can be used to specify things that will
    go into headers of the HTML output documents. The functions below return
    the meta-data content in an `MgInputFile` (this is convenient since we might
    allow meta-data declarations in the ordinary files too), but the file will
    not be added to the linked list of input documents.
    */

    MgInputFile* MgAddMetaDataText(
        MgContext*  context,
        const char* path,
        const char* textBegin,
        const char* textEnd );

    MgInputFile* MgAddMetaDataFile(
        MgContext*  context,
        const char* path );

    /*
    ## Output ##

    As input files are read, Mangle builds up a representation
    of Markdown document structure, and also creates a cross-
    referenced database of literate programming "scraps" that
    are defined or referenced in each file. Scraps with the
    same identifier are grouped together, and some of these
    groups represent source files that should be compiled
    to create a program.

    Once you are done adding input files, you can begin to
    export documentation and code files. The simplest way
    to do this is with `MgWriteAllDocFiles` and
    `MgWriteAllCodeFiles`.
    */

    #ifndef MANGLE_NO_STDIO
    void MgWriteAllDocFiles(
        MgContext* context );

    void MgWriteAllCodeFiles(
        MgContext* context );
    #endif

    /*
    If this isn't suitable for your needs, you can instead
    iterate over the relevant files yourself.

    Input files can be iterated using `MgGetFirstInputFile`
    and `MgGetNextInputFile`, or by just hanging on to
    the pointer returned by `MgAddInputFile*`:
    */

    MgInputFile* MgGetFirstInputFile(
        MgContext* context );

    MgInputFile* MgGetNextInputFile(
        MgInputFile* inputFile );

    /*
    Similarly, you can iterate over the output code files that
    were specified in the literate program, by using
    `MgGetFirstCodeFile` and `MgGetNextCodeFile`.

    Note that output code files are just a special case of
    "scrap groups," so this is really just a specialized
    iterator over a subset of those:
    */
    typedef struct MgScrapNameGroupT MgScrapNameGroup;

    MgScrapNameGroup* MgGetFirstCodeFile(
        MgContext* context );

    MgScrapNameGroup* MgGetNextCodeFile(
        MgScrapNameGroup* codeFile );

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

    void MgWriteDocFile(
        MgContext*      context,
        MgInputFile*    inputFile);

    void MgWriteCodeFile(
        MgContext*          context,
        MgScrapNameGroup*   codeFile );

    /*
    TODO: lower-level routines for output to buffer/FILE*.
    */

    /*
    The following declarations give us the data structures used to represent
    documents that have been parsed by Mangle. A client of the API probably
    doesn't need to see any of this, but it is easier to define it here
    than to try to hide it all.
    */

    typedef struct MgLineT              MgLine;
    typedef struct MgReferenceLinkT     MgReferenceLink;
    typedef struct MgScrapT             MgScrap;
    typedef struct MgScrapFileGroupT    MgScrapFileGroup;
    typedef struct MgElementT           MgElement;

    <<string declarations>>

    /*
    A location inside a source file (one-based line and column numbers).
    */
    typedef struct MgSourceLocT
    {
        int line;
        int col;
    } MgSourceLoc;

    /*
    Kinds of scraps.
    */
    typedef enum MgScrapKind
    {
        kScrapKind_Unknown,     /* no kind declared yet */
        kScrapKind_OutputFile,  /* an output file `file:` */
        kScrapKind_GlobalMacro, /* a global (cross-file) macro `global:` */
        kScrapKind_LocalMacro,  /* a local macro `local:` */
    } MgScrapKind;

    /*
    A single scrap definition.
    */
    struct MgScrapT
    {
        MgScrapFileGroup*   fileGroup;  /* the parent file group */
        MgSourceLoc         sourceLoc;  /* the starting location */
        MgElement*          body;       /* the body of the scrap (as parsed elements) */
        MgScrap*            next;       /* the next scrap in the same file group */
    };

    /*
    A scrap file group represents one or more scraps with the same ID, in the
    same input file.
    */
    struct MgScrapFileGroupT
    {
        MgScrapNameGroup* nameGroup;    /* the parent name group */
        MgInputFile*      inputFile;    /* the file containing all these definitions */
        MgScrap*          firstScrap;   /* the first scrap definition in the file for this ID */
        MgScrap*          lastScrap;    /* the last scrap definition in the file for this ID */
        MgScrapFileGroup* next;         /* the next file group with the same ID */
    };

    /*
    A scrap name group represents one or more scraps with the same ID, grouped by file.
    */
    struct MgScrapNameGroupT
    {
        MgScrapKind         kind;           /* the kind, or `kMgScrapKind_Unknown` if not declared yet */
        MgString            id;             /* the internal ID for the scraps */
        MgElement*          name;           /* the "pretty" name we present to the user */
        MgScrapFileGroup*   firstFileGroup; /* first file group with this ID */
        MgScrapFileGroup*   lastFileGroup;  /* last file group with this ID */
        MgScrapNameGroup*   next;           /* next scrap name group in the context */
    };

    /*
    An input file represents one of the Markdown documents that has been input
    to the Mangle context.
    */
    struct MgInputFileT
    {
        char const*     path;               /* path of input file (terminated) */
        MgString        text;               /* full text of the input file */
        char*           allocatedFileData;  /* allocated file buffer, if any */
        MgLine*         beginLines;         /* allocated per-line data */
        MgLine*         endLines;
        MgElement*      firstElement;       /* first element in doc structure*/
        MgInputFile*    next;               /* next input file in context */
        MgReferenceLink*firstReferenceLink; /* first reference link parsed */
    };

    /*
    The `MgContext` type holds the state of the entire Mangle system.
    */
    struct MgContextT
    {
        MgInputFile*        firstInputFile;         /* singly-linked list of input files */
        MgInputFile*        lastInputFile;

        MgScrapNameGroup*   firstScrapNameGroup;    /* singly-linked list of scrap name groups */
        MgScrapNameGroup*   lastScrapNameGroup;

        MgInputFile*        metaDataFile;
    };

    /*
    Kind of elements that we parse. These match closely to HTML element types
    as well as Markdown document structure.
    */
    typedef enum MgElementKindT
    {
        /* block-level */

        kMgElementKind_Header1,             /* `<h1>` */
        kMgElementKind_Header2,             /* `<h2>` */
        kMgElementKind_Header3,             /* `<h3>` */
        kMgElementKind_Header4,             /* `<h4>` */
        kMgElementKind_Header5,             /* `<h5>` */
        kMgElementKind_Header6,             /* `<h6>` */
        kMgElementKind_BlockQuote,          /* `<blockquote>` */
        kMgElementKind_CodeBlock,           /* `<pre><code>` */
        kMgElementKind_HorizontalRule,      /* `<hr>` */
        kMgElementKind_UnorderedList,       /* `<ul>` */
        kMgElementKind_OrderedList,         /* `<ol>` */
        kMgElementKind_ListItem,            /* `<li>` */
        kMgElementKind_Paragraph,           /* `<p>` */
        kMgElementKind_Table,               /* `<table>` */
        kMgElementKind_TableRow,            /* `<tr>` */
        kMgElementKind_TableHeader,         /* `<th>` */
        kMgElementKind_TableCell,           /* `<td>` */
        kMgElementKind_Text,                /* raw text */
        kMgElementKind_HtmlBlock,           /* raw text, assumed to be HTML */
        kMgElementKind_ScrapDef,            /* literate scrap definition with
                                                children: scrap body
                                                $scrap attribute: MgScrap*
                                            */
        kMgElementKind_MetaData,            /* meta-data declaration with
                                                children: value
                                                $key attribute: key
                                            */

        /* span-level */
        kMgElementKind_NewLine,             /* `"\n"` */
        kMgElementKind_Em,                  /* `<em>` */
        kMgElementKind_Strong,              /* `<strong>` */
        kMgElementKind_InlineCode,          /* `<code>` */
        kMgElementKind_Link,                /* `<a>` with href attribute */
        kMgElementKind_LessThanEntity,      /* `&lt;` */
        kMgElementKind_GreaterThanEntity,   /* `&gt;` */
        kMgElementKind_AmpersandEntity,     /* `&amp;` */
        kMgElementKind_ReferenceLink,       /* `<a>` with
                                                $referenceLink attribute:
                                                    MgReferenceLink*
                                            */
        kMgElementKind_ScrapRef,            /* reference back to a literate
                                               scrap, with
                                                $scrap-group attribute:
                                                    MgScrapFileGroup*
                                                $resume-at attribute:
                                                    MgSourceLoc after end of ref
                                            */
    } MgElementKind;

    /*
    A reference link structure is used to track the association between a
    reference site for a reference-style link like `[Foo][]` and the
    associated definition `[Foo] http://foo.com "Foo"`.

    It is created whenever a def/ref is encountered, and filled in with
    whatever information is available at the def/ref site. Later defs/refs
    for the same ID (case-insensitive) will refer to the same structure.
    */
    struct MgReferenceLinkT
    {
        MgString          id;
        MgString          url;
        MgString          title;
        MgReferenceLink*  next;
    };

    /*
    An attribute structure is used to represent auxiliary data attached
    to an element. This is both used for HTML attributes (in which case
    `val` holds the text of the attriute balue), and for other kinds
    of auxiliary data (in which case the ID will start with `$`, and the
    data will be in one of the other fields of the `union`).
    */
    typedef struct MgAttributeT MgAttribute;
    struct MgAttributeT
    {
        MgString              id;
        MgAttribute*          next;
        union
        {
            MgString          val;
            MgReferenceLink*  referenceLink;
            MgScrap*          scrap;
            MgScrapFileGroup* scrapFileGroup;
            MgSourceLoc       sourceLoc;
        };
    };

    /*
    An element represents a part of the Markdown or HTML document structure.
    Each eleemnt has a `kind` that effectively tells us what the HTML tag
    would be, or otherwise identifies the flavor of content, some optional
    direct `text`, zero or more attributes, and zero or more children.
    */
    struct MgElementT
    {
        MgElementKind   kind;

        MgString        text;

        MgAttribute*    firstAttr;
        MgElement*      firstChild;

        MgElement*      next;
    };


    /*

    Implementation
    ==============

    After this point we have the implementation of the above API,
    and then after *that* we have a driver application.
    */

    <<utility declarations>>

    <<reader definitions>>

    <<string definitions>>

    <<parsing definitions>>

    <<span-level parsing definitions>>

    <<block-level parsing definitions>>

    <<writer definitions>>

    <<export definitions>>

    <<code export definitions>>

    <<HTML export definitions>>

    MgInputFile* MgGetFirstInputFile(
        MgContext* context)
    {
        if( !context ) return 0;
        return context->firstInputFile;
    }

    MgInputFile* MgGetNextInputFile(
        MgInputFile*  inputFile)
    {
        if( !inputFile ) return 0;
        return inputFile->next;
    }

    void MgWriteAllDocFiles(
        MgContext* context )
    {
        MgInputFile* inputFile = MgGetFirstInputFile( context );
        while( inputFile )
        {
            MgWriteDocFile( context, inputFile );
            inputFile = MgGetNextInputFile( inputFile );
        }
    }





    /* Core Compilation Flow */

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

    void MgInitialize(
        MgContext*  context )
    {
        memset(context, 0, sizeof(*context));
    }

    void MgFinalizeElements(
        MgContext*      context,
        MgElement*      firstElement );

    void MgFinalizeElement(
        MgContext*      context,
        MgElement*      element )
    {
        MgFinalizeElements( context, element->firstChild );

        MgAttribute* attr = element->firstAttr;
        while( attr )
        {
            MgAttribute* next = attr->next;
            free( attr );
            attr = next;
        }
    }

    void MgFinalizeElements(
        MgContext*      context,
        MgElement*      firstElement )
    {
        MgElement* element = firstElement;
        while( element )
        {
            MgElement* next = element->next;
            MgFinalizeElement( context, element );
            free( element );
            element = next;
        }
    }

    void MgFinalizeInputFile(
        MgContext*      context,
        MgInputFile*    file)
    {
        if( file->allocatedFileData )
            free( file->allocatedFileData );
        if( file->beginLines )
            free( file->beginLines );

        MgFinalizeElements( context, file->firstElement );

        MgReferenceLink* link = file->firstReferenceLink;
        while( link )
        {
            MgReferenceLink* next = link->next;
            free(link);
            link = next;
        }
    }

    void MgFinalizeScrapFileGroup(
        MgContext*          context,
        MgScrapFileGroup*   group)
    {
        MgScrap* scrap = group->firstScrap;
        while( scrap )
        {
            MgScrap* next = scrap->next;
            // NOTE: the `body` of a scrap is already
            // stored elsewhere in the document structure,
            // so we don't free it here...
            free( scrap );
            scrap = next;
        }
    }

    void MgFinalizeScrapNameGroup(
        MgContext*          context,
        MgScrapNameGroup*   group)
    {
        MgScrapFileGroup* subGroup = group->firstFileGroup;
        while( subGroup )
        {
            MgScrapFileGroup* next = subGroup->next;
            MgFinalizeScrapFileGroup( context, subGroup );
            free( subGroup );
            subGroup = next;
        }
    }

    void MgFinalize(
        MgContext*  context)
    {
        MgInputFile* file = context->firstInputFile;
        while( file )
        {
            MgInputFile* next = file->next;
            MgFinalizeInputFile( context, file );
            free( file );
            file = next;
        }

        MgScrapNameGroup* group = context->firstScrapNameGroup;
        while( group )
        {
            MgScrapNameGroup* next = group->next;
            MgFinalizeScrapNameGroup( context, group );
            free( group );
            group = next;
        }
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
            return MG_NULL;
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
            return MG_NULL;

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
            return MG_NULL;
        }
        // we NULL-terminate the buffer just in case
        // (but the code should never rely on this)
        fileData[size] = 0;

        int sizeRead = fread(fileData, 1, size, stream);
        if( sizeRead != size )
        {
            fprintf(stderr, "failed to read from \"%s\"\n", path);
            free(fileData);
            return MG_NULL;
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
            return MG_NULL;

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
        FILE* stream = MG_NULL;
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
            return MG_NULL;

        MgInputFile* inputFile = MgAllocateInputFile(
            context,
            path,
            textBegin,
            textEnd );
        if( !inputFile )
            return MG_NULL;

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
            return MG_NULL;

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
        FILE* stream = MG_NULL;
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

    /*

    Application
    ===========

    */

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

    <<`main` function>>


The Main Entry Point
====================


    <<`main` function>>=
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

