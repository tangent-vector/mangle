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
#ifndef MANGLE_H_INCLUDED
#define MANGLE_H_INCLUDED

/*
# Mangle: A Literate Programming Tool #

Mangle is a tool for literate programming using Markdown
together with C-like languages. Please see the README.md
file if you want to know how to use Mangle as a command-
line tool, and to understand the kind of syntax it expects.
This header file will only document the library interface
to Mangle's internals, for the benefit of anybody who
wants to build on or improve it.

## Compile-Time Configuration ##

If you don't want `mangle.h` to include `<stdio.h>`, then you
will want to `#define MANGLE_NO_STDIO` before including it.

If you don't want the *implementation* of Mangle to make use
of <stdio.h>, you'll want to define it globally, either as
a command-line option to your build, or by modifying this file.
*/
#ifndef MANGLE_NO_STDIO
#include <stdio.h>
#endif

/*
Mangle is implemented in C, but should be usable from both C and C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

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

#ifndef MANGLE_NO_STDIO
MgInputFile* MgAddInputFileStream(
    MgContext*  context,
    const char* path,
    FILE*       stream );

MgInputFile* MgAddInputFilePath(
    MgContext*  context,
    const char* path );
#endif

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

#ifndef MANGLE_NO_STDIO
void MgWriteDocFile(
    MgContext*      context,
    MgInputFile*    inputFile);

void MgWriteCodeFile(
    MgContext*          context,
    MgScrapNameGroup*   codeFile );
#endif

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

/*
A sub-range of an existing string buffer.
*/
typedef struct MgStringT
{
    char const* begin;
    char const* end;
} MgString;

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

    /* span-level */
    kMgElementKind_Em,                  /* `<em>` */
    kMgElementKind_Strong,              /* `<strong>` */
    kMgElementKind_InlineCode,          /* `<code>` */
    kMgElementKind_Link,                /* `<a>` with href attribute */
    kMgElementKind_LessThanEntity,      /* `&lt;` */
    kMgElementKind_GreaterThanEntity,   /* `&gt;` */
    kMgElementKind_AmpersandEntity,     /* `&amp;` */
    kMgElementKind_ReferenceLink,       /* `<a>` with
                                            $referenceLink attribute: MgReferenceLink*
                                        */
    kMgElementKind_ScrapRef,            /* reference back to a literate scrap, with
                                            $scrap-group attribute: MgScrapFileGroup*
                                            $resume-at attribute: MgSourceLoc after end of reference
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

#ifdef __cplusplus
}
#endif

#endif /* MANGLE_H_INCLUDED */
