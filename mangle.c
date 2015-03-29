
#line 123 "source/main.md"
    
#line 50 "README.md"
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
    
#line 123 "source/main.md"
               
    
#line 135 "source/main.md"
    #include <assert.h>
    #include <ctype.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
#line 124 "source/main.md"
                
    
#line 147 "source/main.md"
    
#line 11 "source/string.md"
    typedef struct MgStringT
    {
        char const* begin;
        char const* end;
    } MgString;
    
#line 43 "source/string.md"
    MgString MgMakeString( char const* begin, char const* end );
    
#line 147 "source/main.md"
                           
    
#line 484 "source/document.md"
    
#line 473 "source/document.md"
    typedef struct MgAttributeT         MgAttribute;
    typedef struct MgContextT           MgContext;
    typedef struct MgElementT           MgElement;
    typedef struct MgInputFileT         MgInputFile;
    typedef struct MgLineT              MgLine;
    typedef struct MgReferenceLinkT     MgReferenceLink;
    typedef struct MgScrapT             MgScrap;
    typedef struct MgScrapFileGroupT    MgScrapFileGroup;
    typedef struct MgScrapNameGroupT    MgScrapNameGroup;
    
#line 484 "source/document.md"
                                     
    
#line 13 "source/document.md"
    typedef int MgBool;
    #define MG_TRUE     (1)
    #define MG_FALSE    (0)
    
#line 24 "source/document.md"
    typedef struct MgSourceLocT
    {
        int line;
        int col;
    } MgSourceLoc;
    
#line 40 "source/document.md"
    struct MgScrapT
    {
        
#line 48 "source/document.md"
    MgSourceLoc         sourceLoc;
    MgElement*          body;
    
#line 118 "source/document.md"
    MgScrap*            next;
    
#line 127 "source/document.md"
    MgScrapFileGroup*   fileGroup;
    
#line 42 "source/document.md"
                         
    };
    
#line 56 "source/document.md"
    typedef enum MgScrapKind
    {
        
#line 68 "source/document.md"
    kScrapKind_Unknown,
    
#line 79 "source/document.md"
    kScrapKind_LocalMacro,
    
#line 88 "source/document.md"
    kScrapKind_GlobalMacro,
    
#line 97 "source/document.md"
    kScrapKind_OutputFile,
    
#line 58 "source/document.md"
                       
    } MgScrapKind;
    
#line 104 "source/document.md"
    struct MgScrapFileGroupT
    {
        
#line 112 "source/document.md"
    MgInputFile*      inputFile;
    
#line 121 "source/document.md"
    MgScrap*          firstScrap;
    MgScrap*          lastScrap;
    
#line 159 "source/document.md"
    MgScrapFileGroup* next;
    
#line 168 "source/document.md"
    MgScrapNameGroup* nameGroup;
    
#line 106 "source/document.md"
                                    
    };
    
#line 134 "source/document.md"
    struct MgScrapNameGroupT
    {
        
#line 144 "source/document.md"
    MgString            id;
    MgElement*          name;
    
#line 150 "source/document.md"
    MgScrapKind         kind;
    
#line 162 "source/document.md"
    MgScrapFileGroup*   firstFileGroup;
    MgScrapFileGroup*   lastFileGroup;
    
#line 177 "source/document.md"
    MgScrapNameGroup*   next;
    
#line 136 "source/document.md"
                                    
    };
    
#line 185 "source/document.md"
    struct MgLineT
    {
        MgString      text;
        char const* originalBegin;
    };
    
#line 211 "source/document.md"
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
    
#line 230 "source/document.md"
    struct MgContextT
    {
        MgInputFile*        firstInputFile;         /* singly-linked list of input files */
        MgInputFile*        lastInputFile;
    
        MgScrapNameGroup*   firstScrapNameGroup;    /* singly-linked list of scrap name groups */
        MgScrapNameGroup*   lastScrapNameGroup;
    
        MgInputFile*        metaDataFile;
    };
    
#line 250 "source/document.md"
    typedef enum MgElementKindT
    {
        
#line 258 "source/document.md"
    
#line 266 "source/document.md"
    kMgElementKind_BlockQuote,          /* `<blockquote>` */
    kMgElementKind_HorizontalRule,      /* `<hr>` */
    kMgElementKind_UnorderedList,       /* `<ul>` */
    kMgElementKind_OrderedList,         /* `<ol>` */
    kMgElementKind_ListItem,            /* `<li>` */
    kMgElementKind_Paragraph,           /* `<p>` */
    kMgElementKind_Table,               /* `<table>` */
    kMgElementKind_TableRow,            /* `<tr>` */
    kMgElementKind_TableHeader,         /* `<th>` */
    kMgElementKind_TableCell,           /* `<td>` */
    
#line 287 "source/document.md"
    kMgElementKind_Header1,             /* `<h1>` */
    kMgElementKind_Header2,             /* `<h2>` */
    kMgElementKind_Header3,             /* `<h3>` */
    kMgElementKind_Header4,             /* `<h4>` */
    kMgElementKind_Header5,             /* `<h5>` */
    kMgElementKind_Header6,             /* `<h6>` */
    
#line 300 "source/document.md"
    kMgElementKind_CodeBlock,           /* `<pre><code>` */
    
#line 307 "source/document.md"
    kMgElementKind_ScrapDef,
    
#line 323 "source/document.md"
    kMgElementKind_MetaData,
    
#line 331 "source/document.md"
    kMgElementKind_HtmlBlock,
    
#line 258 "source/document.md"
                                 
    
#line 278 "source/document.md"
    kMgElementKind_Em,                  /* `<em>` */
    kMgElementKind_Strong,              /* `<strong>` */
    kMgElementKind_InlineCode,          /* `<code>` */
    
#line 316 "source/document.md"
    kMgElementKind_ScrapRef,
    
#line 345 "source/document.md"
    kMgElementKind_LessThanEntity,      /* `&lt;` */
    kMgElementKind_GreaterThanEntity,   /* `&gt;` */
    kMgElementKind_AmpersandEntity,     /* `&amp;` */
    
#line 355 "source/document.md"
    kMgElementKind_NewLine,             /* `"\n"` */
    
#line 362 "source/document.md"
    kMgElementKind_Link,                /* `<a>` with href attribute */
    
#line 388 "source/document.md"
    kMgElementKind_ReferenceLink,
    
#line 259 "source/document.md"
                                
    
#line 338 "source/document.md"
    kMgElementKind_Text,
    
#line 252 "source/document.md"
                         
    } MgElementKind;
    
#line 375 "source/document.md"
    struct MgReferenceLinkT
    {
        MgString          id;
        MgString          url;
        MgString          title;
        MgReferenceLink*  next;
    };
    
#line 396 "source/document.md"
    struct MgAttributeT
    {
        
#line 410 "source/document.md"
    MgString              id;
    
#line 415 "source/document.md"
    MgAttribute*          next;
    
#line 398 "source/document.md"
                             
        union
        {
            
#line 420 "source/document.md"
    MgString          val;
    
#line 425 "source/document.md"
    MgReferenceLink*  referenceLink;
    MgScrap*          scrap;
    MgScrapFileGroup* scrapFileGroup;
    MgSourceLoc       sourceLoc;
    
#line 401 "source/document.md"
                                       
        };
    };
    
#line 435 "source/document.md"
    struct MgElementT
    {
        
#line 443 "source/document.md"
    MgElementKind   kind;
    
#line 449 "source/document.md"
    MgString        text;
    
#line 454 "source/document.md"
    MgAttribute*    firstAttr;
    
#line 459 "source/document.md"
    MgElement*      firstChild;
    MgElement*      next;
    
#line 437 "source/document.md"
                           
    };
    
#line 485 "source/document.md"
                                  
    
#line 148 "source/main.md"
                             
    
#line 125 "source/main.md"
                    
    
#line 153 "source/main.md"
    
#line 13 "source/reader.md"
    typedef struct MgReaderT
    {
        MgString    string;
        char const* cursor;
    } MgReader;
    
#line 23 "source/reader.md"
    enum
    {
        kMgEndOfFile = -1,
    };
    
#line 31 "source/reader.md"
    void MgInitializeStringReader(
        MgReader*   reader,
        MgString          string )
    {
        reader->string  = string;
        reader->cursor  = string.begin;    
    }
    
#line 42 "source/reader.md"
    MgBool MgAtEnd(
        MgReader*   reader )
    {
        return reader->cursor == reader->string.end;
    }
    
#line 52 "source/reader.md"
    int MgGetChar(
        MgReader*   reader )
    {
        if( MgAtEnd(reader) )
            return kMgEndOfFile;
    
        return *(reader->cursor++);
    }
    
#line 65 "source/reader.md"
    void MgUnGetChar(
        MgReader*   reader,
        int             value )
    {
        if( value == kMgEndOfFile )
            return;
    
        --(reader->cursor);
    }
    
#line 81 "source/reader.md"
    int MgPeekChar(
        MgReader*   reader )
    {
        if( MgAtEnd(reader) )
            return kMgEndOfFile;
    
        return *(reader->cursor);
    }
    
#line 153 "source/main.md"
                          
    
#line 23 "source/string.md"
    static MgBool MgIsEmptyString(
        MgString string)
    {
        return string.begin == string.end;
    }
    
#line 32 "source/string.md"
    MgString MgMakeEmptyString()
    {
        return MgMakeString(NULL, NULL);
    }
    
#line 46 "source/string.md"
    MgString MgMakeString(
        char const* begin,
        char const* end)
    {
        MgString result;
        result.begin    = begin;
        result.end      = end;
        return result;
    }
    
#line 60 "source/string.md"
    MgString MgTerminatedString(
        char const* begin)
    {
        return MgMakeString(begin, begin + strlen(begin));
    }
    
#line 74 "source/string.md"
    MgBool MgStringsAreEqual(
        MgString left,
        MgString right )
    {
        char const* leftCursor = left.begin;
        char const* rightCursor = right.begin;
    
        for(;;)
        {
            
#line 93 "source/string.md"
    MgBool leftAtEnd = leftCursor == left.end;
    MgBool rightAtEnd = rightCursor == right.end;
    if( leftAtEnd || rightAtEnd )
        return leftAtEnd == rightAtEnd;
    
#line 83 "source/string.md"
                                        
            
#line 101 "source/string.md"
    char leftChar = *leftCursor++;
    char rightChar = *rightCursor++;
    
#line 84 "source/string.md"
                                          
            
#line 105 "source/string.md"
    if( leftChar != rightChar )
        return MG_FALSE;
    
#line 85 "source/string.md"
                                               
        }
    }
    
#line 113 "source/string.md"
    MgBool MgStringsAreEqualNoCase(
        MgString left,
        MgString right )
    {
        char const* leftCursor = left.begin;
        char const* rightCursor = right.begin;
    
        for(;;)
        {
            
#line 93 "source/string.md"
    MgBool leftAtEnd = leftCursor == left.end;
    MgBool rightAtEnd = rightCursor == right.end;
    if( leftAtEnd || rightAtEnd )
        return leftAtEnd == rightAtEnd;
    
#line 122 "source/string.md"
                                        
            
#line 101 "source/string.md"
    char leftChar = *leftCursor++;
    char rightChar = *rightCursor++;
    
#line 123 "source/string.md"
                                          
            
#line 129 "source/string.md"
    if( tolower(leftChar) != tolower(rightChar) )
        return MG_FALSE;
    
#line 124 "source/string.md"
                                                                
        }
    }
    
#line 154 "source/main.md"
                          
    
#line 5 "source/parse.md"
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
    
#line 155 "source/main.md"
                           
    
#line 5 "source/parse-span.md"
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
        MgElement* element = NULL;
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
        MgElement* inner = NULL;
        if( flags & kMgSpanFlag_DontProcessMarkdown )
            return NULL;
    
        // follow GitHub Flavored Markdown, in only
        // allowing underscores for <em> when
        // they mark a whole word...
    
        // we need to look at the character before `c`
        if( reader->cursor > inputFile->text.begin )
        {
            char prev = *(reader->cursor - 1);
            if( (c == '_') && !isspace(prev) )
                return NULL;            
        }
    
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
            return NULL;
    
        int e = MgPeekChar( reader );
        if( isspace(e) )
            return NULL; // can't start with white-space
    
        // appears to be the start of a span.
        // now we need to find the matching marker(s)
        char const* start = reader->cursor;
        char const* end = MgFindMatching( reader, c, count );
        if( !end )
            return NULL;
    
        if( (c == '_') && isalpha(MgPeekChar(reader)) )
            return NULL;
    
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
        MgElement* inner = NULL;
        if( flags & kMgSpanFlag_DontProcessMarkdown )
            return NULL;
    
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
            return NULL;
    
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
            return NULL;
    
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
            MgElement* inner    = NULL;
            MgElement* link     = NULL;
    
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
    
#line 156 "source/main.md"
                                      
    
#line 2105 "source/parse-block.md"
    
#line 34 "source/parse-block.md"
    typedef struct LineRangeT
    {
        MgLine* begin;
        MgLine* end;
    } LineRange;
    
#line 101 "source/parse-block.md"
    #define BLOCK_PARSE_FUNC(Name) \
        MgElement* Name( MgContext* context, MgInputFile* inputFile, LineRange* ioLineRange )
    typedef BLOCK_PARSE_FUNC((*BlockParseFunc));
    
#line 2105 "source/parse-block.md"
                                 
    
#line 21 "source/parse-block.md"
    MgElement* ParseBlockElement(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange*      ioLineRange );
    
    MgElement* ParseBlockElementsInRange(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       lineRange );
    
#line 342 "source/parse-block.md"
    MgElement* ParseSetextHeader(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange*      ioLineRange,
        char            c,
        MgElementKind   kind );
    
#line 869 "source/parse-block.md"
    MgElement* ParseCodeBlockBody(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       inLineRange,
        char const*     langBegin,
        char const*     langEnd );
    
#line 2097 "source/parse-block.md"
    char const* CheckIndentedCodeLine(
        MgLine* line );
    
#line 2106 "source/parse-block.md"
                                        
    
#line 272 "source/parse-block.md"
    MgBool IsBlankLine( MgLine* line )
    {
        char const* cursor = line->text.begin;
        char const* end = line->text.end;
        while( cursor != end )
        {
            if( !isspace(*cursor) )
                return MG_FALSE;
            ++cursor;
        }
        return MG_TRUE;
    }
    
#line 1913 "source/parse-block.md"
    void SkipEmptyLines(
        LineRange*  ioLineRange )
    {
        for(;;)
        {
            MgLine* begin = ioLineRange->begin;
            if( begin == ioLineRange->end )
                return;
            if( !IsBlankLine(begin) )
                return;
                
            ioLineRange->begin = begin + 1;
        }
    }
    
#line 1931 "source/parse-block.md"
    MgElement* ReadSpansInRange(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       lines,
        MgSpanFlags       flags)
    {
        return MgReadSpanElementsFromLines(
            context,
            inputFile,
            lines.begin,
            lines.end,
            flags );
    }
    
    MgBool LineIsAll(
        MgLine* line,
        char    c )
    {
        char const* cursor  = line->text.begin;
        char const* end     = line->text.end;
        if( cursor == end )
            return MG_FALSE;
        while( cursor != end )
        {
            if( *cursor != c )
            {
                return MG_FALSE;
            }
            ++cursor;
        }
        return MG_TRUE;
    }
    
    MgLine* GetLine(
        LineRange*  ioLineRange )
    {
        if( ioLineRange->begin == ioLineRange->end )
            return 0;
    
        return ioLineRange->begin++;
    }
    
    void UnGetLine(
        LineRange*  ioLineRange,
        MgLine*     line )
    {
        if( !line ) return;
        assert(line == (ioLineRange->begin-1));
        ioLineRange->begin--;
    }
    
    LineRange MgInclusiveLineRange(
        MgLine* first,
        MgLine* last)
    {
        LineRange range = { first, last + 1 };
        return range;
    }
    
    LineRange Snip(
        MgLine*     firstLine,
        MgLine*     lastLine,
        LineRange*  ioLineRange )
    {
        assert(firstLine);
        assert(lastLine);
        ioLineRange->begin = lastLine + 1;
        return MgInclusiveLineRange( firstLine, lastLine );
    }
    
    //
    //
    //
    
    
    
    
    
    void TrimLeadingSpace(
        char const** ioBegin,
        char const* end )
    {
        char const* begin = *ioBegin;
        while( begin != end && isspace(*begin) )
            ++begin;
        *ioBegin = begin;
    }
    
    void TrimTrailingSpace(
        char const* begin,
        char const** ioEnd )
    {
        char const* end = *ioEnd;
        while( end != begin && isspace(*(end -1)) )
            --end;
        *ioEnd = end;
    }
    
    void TrimTrailingChar(
        char const* begin,
        char const** ioEnd,
        char            c )
    {
        char const* end = *ioEnd;
        while( end != begin && (*(end -1) == c) )
            --end;
        *ioEnd = end;
    }
    
    void InitializeLineReader(
        MgReader*   reader,
        MgLine*         line )
    {
        MgInitializeStringReader( reader, line->text );
    }
    
    int GetIndent(
        MgLine* line )
    {
        char const* cursor = line->text.begin;
        char const* end = line->text.end;
    
        int indent = 0;
        while( cursor != end )
        {
            char c = *cursor;
            if( c == ' ' )
            {
                indent += 1;
            }
            else if( c == '\t' )
            {
                indent += 4;
            }
            else
            {
                break;
            }
            ++cursor;
        }
        return indent;
    }
    
    MgString CString(char const* text)
    {
        MgString string = { text, text + strlen(text) };
        return string;
    }
    
    static void SkipWhiteSpace(
        MgReader* reader )
    {
        for(;;)
        {
            int c = MgGetChar(reader);
            if( !isspace(c) )
            {
                MgUnGetChar(reader, c);
                return;
            }
        }
    }
    
#line 2107 "source/parse-block.md"
                                     
    
#line 46 "source/parse-block.md"
    MgElement* ParseBlockElementsInRange(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       lineRange )
    {
        MgElement*  elements    = NULL;
        MgElement** elementLink = &elements;
    
        for(;;)
        {
            
#line 67 "source/parse-block.md"
    SkipEmptyLines(&lineRange);
    
#line 75 "source/parse-block.md"
    if( lineRange.begin == lineRange.end )
        break;
    
#line 68 "source/parse-block.md"
                               
    
#line 81 "source/parse-block.md"
    MgElement* element = ParseBlockElement( context, inputFile, &lineRange );
    
#line 69 "source/parse-block.md"
                                    
    
#line 86 "source/parse-block.md"
    *elementLink = element;
    while( *elementLink )
        elementLink = &(*elementLink)->next;
    
#line 70 "source/parse-block.md"
                                              
    
#line 56 "source/parse-block.md"
                                                                 
        }
    
        return elements;
    }
    
#line 210 "source/parse-block.md"
    MgElement* ParseBlockLevelHtml(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        MgLine* firstLine = GetLine( ioLineRange );
        MgLine* lastLine = firstLine;
    
        MgReader reader;
        InitializeLineReader( &reader, firstLine );
    
        int c = MgGetChar( &reader );
        if( c != '<' ) return NULL;
    
        int d = MgGetChar( &reader );
        if( !isalpha(d) ) return NULL;
    
        for(;;)
        {
            MgLine* line = GetLine( ioLineRange );
            if( !line )
                break;
    
            lastLine = line;
    
            InitializeLineReader( &reader, line );
            int e = MgGetChar( &reader );
            if( e != '<' )
                continue;
            int f = MgGetChar( &reader );
            if( f != '/' )
                continue;
            int g = MgGetChar( &reader );
            if( !isalpha(g) )
                continue;
    
            break;
        }
    
        LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );
    
        MgElement* firstChild = ReadSpansInRange( context, inputFile, innerRange, kMgSpanFlags_HtmlBlock );
        return MgCreateParentElement(
            kMgElementKind_HtmlBlock,
            firstChild );
    }
    
#line 288 "source/parse-block.md"
    BLOCK_PARSE_FUNC(ParseDefaultParagraph)
    {
        MgLine* firstLine = GetLine( ioLineRange );
        MgLine* lastLine = firstLine;
    
        for(;;)
        {
            MgLine* line = GetLine( ioLineRange );
            if( !line || IsBlankLine(line) )
            {
                UnGetLine(ioLineRange, line);
                break;
            }
    
            lastLine = line;
        }
    
        LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );
    
        MgElement* firstChild = ReadSpansInRange( context, inputFile, innerRange, kMgSpanFlags_Default );
        return MgCreateParentElement(
            kMgElementKind_Paragraph,
            firstChild );
    }
    
#line 353 "source/parse-block.md"
    BLOCK_PARSE_FUNC(ParseSetextHeader1)
    {
        return ParseSetextHeader(
            context, inputFile,
            ioLineRange,
            '=',
            kMgElementKind_Header1 );
    }
    
    BLOCK_PARSE_FUNC(ParseSetextHeader2)
    {
        return ParseSetextHeader(
            context, inputFile,
            ioLineRange,
            '-',
            kMgElementKind_Header2 );
    }
    
#line 377 "source/parse-block.md"
    MgElement* ParseSetextHeader(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange,
        char            c,
        MgElementKind   kind )
    {
        
#line 408 "source/parse-block.md"
    MgLine* firstLine = GetLine(ioLineRange);
    MgLine* secondLine = GetLine(ioLineRange);
    if( !secondLine ) return 0;
    
#line 384 "source/parse-block.md"
                                 
    
        
#line 419 "source/parse-block.md"
    if(!LineIsAll(secondLine, c))
        return 0;
    
#line 386 "source/parse-block.md"
                                                      
    
        // the inner range does not include the second line,
        // so we can't just use the Snip() function for everything
        LineRange innerRange = MgInclusiveLineRange(firstLine, firstLine);
        Snip( firstLine, secondLine, ioLineRange );
    
        MgElement* firstChild = ReadSpansInRange(
            context,
            inputFile,
            innerRange,
            kMgSpanFlags_Default );
    
        return MgCreateParentElement(
            kind,
            firstChild );
    }
    
#line 449 "source/parse-block.md"
    MgElement* ParseAtxHeader(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        LineRange innerRange;
        MgLine* firstLine = GetLine( ioLineRange );
    
        MgReader reader;
        InitializeLineReader(&reader, firstLine);
    
        int level = 0;
        for(;;)
        {
            int c = MgGetChar(&reader);
            if( c != '#' )
            {
                MgUnGetChar(&reader, c);
                break;
            }
            ++level;
        }
    
        if( level == 0 ) return 0;
    
        firstLine->text.begin = reader.cursor;
    
        TrimLeadingSpace( &firstLine->text.begin, firstLine->text.end );
        TrimTrailingChar( firstLine->text.begin, &firstLine->text.end, '#' );
        TrimTrailingSpace( firstLine->text.begin, &firstLine->text.end );
    
        if( level > kMaxHeaderLevel )
            level = kMaxHeaderLevel;
    
        innerRange = Snip( firstLine, firstLine, ioLineRange );
    
        MgElement* firstChild = ReadSpansInRange( context, inputFile, innerRange, kMgSpanFlags_Default );
    
        return MgCreateParentElement(
            (MgElementKind) (kMgElementKind_Header1 + (level-1)),
            firstChild );
    }
    
#line 542 "source/parse-block.md"
    char const* CheckQuoteLine(
        MgLine* line )
    {
        MgReader reader;
        InitializeLineReader(&reader, line );
    
        int c = MgGetChar( &reader );
        if( c != '>' )
            return 0;
    
        int d = MgGetChar( &reader );
        if( d != ' ' )
        {
            MgUnGetChar( &reader, d );
        }
        return reader.cursor;
    }
    
    MgElement* ParseBlockQuote(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        MgLine* firstLine = GetLine( ioLineRange );
        MgLine* lastLine = firstLine;
    
        MgLine* line = firstLine;
        for(;;)
        {
            if( !line )
                break;
    
            char const* lineStart = CheckQuoteLine(line);
            if( !lineStart )
            {
                if( line == firstLine )
                    return 0; // this isn't a block quote at all!
                else
                    break; // we've found the first line that doesn't belong
            } 
    
            // we are starting a paragraph within the block quote
    
            // continue consuming lines until we see an empty line
            while( line && !IsBlankLine(line) )
            {
                lineStart = CheckQuoteLine(line);
                if( lineStart )
                    line->text.begin = lineStart;
                lastLine = line;
                line = GetLine( ioLineRange );
            }
    
            // continue consuming lines until we see a non-empty line
            while( line && IsBlankLine(line) )
            {
                line = GetLine( ioLineRange );
            }
        }
    
        LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );
    
        MgElement* firstChild = ParseBlockElementsInRange(context, inputFile, innerRange);
        return MgCreateParentElement(
            kMgElementKind_BlockQuote,
            firstChild );
    }
    
#line 618 "source/parse-block.md"
    char const* CheckUnorderedListLine(
        MgLine* line )
    {
        MgReader reader;
        InitializeLineReader( &reader, line );
    
        // may have up to three leading spaces
        for(int ii = 0; ii < 3; ++ii )
        {
            int cc = MgGetChar( &reader );
            if( cc != ' ' )
            {
                MgUnGetChar( &reader, cc );
                break;
            }
        }
    
        int c = MgGetChar( &reader );
        switch( c )
        {
        case '*':
        case '+':
        case '-':
            break;
    
        default:
            return 0;
        }
    
        // skip white-space after the bullet
        for(;;)
        {
            int d = MgGetChar( &reader );
            if( !isspace(d) )
            {
                MgUnGetChar( &reader, d );
                break;
            }
        }
    
        return reader.cursor;
    }
    
    char const* CheckOrderedListLine(
        MgLine* line )
    {
        MgReader reader;
        InitializeLineReader( &reader, line );
    
        // may have up to three leading spaces
        for(int ii = 0; ii < 3; ++ii )
        {
            int cc = MgGetChar( &reader );
            if( cc != ' ' )
            {
                MgUnGetChar( &reader, cc );
                break;
            }
        }
    
        int c = MgGetChar( &reader );
        if( !isdigit(c) )
            return 0;
    
        // skip remaining digits
        for(;;)
        {
            int d = MgGetChar( &reader );
            if( !isdigit(d) )
            {
                MgUnGetChar( &reader, d );
                break;
            }
        }
    
        // expect a dot
        int e = MgGetChar( &reader );
        if( e != '.' )
            return 0;
    
        // skip white-space after the dot
        for(;;)
        {
            int d = MgGetChar( &reader );
            if( !isspace(d) )
            {
                MgUnGetChar( &reader, d );
                break;
            }
        }
    
        return reader.cursor;
    }
    
    char const* CheckListLineLeadingSpace(
        MgLine* line )
    {
        MgReader reader;
        InitializeLineReader( &reader, line );
    
        for( int ii = 0; ii < 4; ++ii )
        {
            int c = MgGetChar( &reader );
            if( c == '\t' )
                break;
            if( c != ' ' )
            {
                MgUnGetChar( &reader, c );
                return 0;
            }
        }
        return reader.cursor;
    }
    
    MgElement* ParseListItem(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange,
        char const* (*checkLineFunc)(MgLine*) )
    {
        MgLine* firstLine = GetLine( ioLineRange );
        MgLine* lastLine = firstLine;
    
        if( !firstLine )
            return 0;
    
        // check the first line
        char const* lineStart = checkLineFunc(firstLine);
        if( !lineStart )
            return 0;
        firstLine->text.begin = lineStart;
    
        // read subsequent lines of the item, until
        MgLine* line = firstLine;
    
        for(;;)
        {
            line = GetLine( ioLineRange );
            while( line )
            {
                // we see an empty line, or
                if( IsBlankLine(line) )
                    break;
    
                // a line that starts a new item
                // \todo: does this need to consider other list flavors?
                lineStart = checkLineFunc(line);
                if(lineStart)
                    break;
    
                lineStart = CheckListLineLeadingSpace(line);
                if( lineStart )
                    line->text.begin = lineStart;
    
                // \todo: need to trim front of line...
    
                lastLine = line;
                line = GetLine(ioLineRange);
            }
    
            // continue consuming lines until we see a non-empty line
            while( line && IsBlankLine(line) )
            {
                line = GetLine(ioLineRange);
            }
    
            // if the next line is indented appropriately for line
            // continuation... we continue building out the same item...
            if( line )
            {
                lineStart = CheckIndentedCodeLine( line );
                if( lineStart )
                {
                    line->text.begin = lineStart;
                    lastLine = line;
                    continue;
                }
            }
            break;
        }
    
        LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );
    
        MgElement* firstChild = ParseBlockElementsInRange( context, inputFile, innerRange );
    
        return MgCreateParentElement(
            kMgElementKind_ListItem,
            firstChild );
    }
    
    MgElement* ParseList(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange,
        MgElementKind kind,
        char const* (*checkLineFunc)(MgLine*) )
    {
        MgElement* firstItem = ParseListItem( context, inputFile, ioLineRange, checkLineFunc );
        MgElement* lastItem = firstItem;
    
        if( !firstItem ) return 0;
    
        for(;;)
        {
            MgElement* item = ParseListItem( context, inputFile, ioLineRange, checkLineFunc );
            if( !item )
                break;
    
            lastItem->next = item;
            lastItem = item;
        }
    
        return MgCreateParentElement(
            kind,
            firstItem );
    }
    
    MgElement* ParseOrderedList(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        return ParseList(
            context, inputFile, 
            ioLineRange,
            kMgElementKind_OrderedList,
            &CheckOrderedListLine );
    }
    
    MgElement* ParseUnorderedList(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        return ParseList(
            context, inputFile,
            ioLineRange,
            kMgElementKind_UnorderedList,
            &CheckUnorderedListLine );
    }
    
#line 893 "source/parse-block.md"
    char const* CheckIndentedCodeLine(
        MgLine* line )
    {
        // either a tab or four spaces
        MgReader reader;
        InitializeLineReader(&reader, line );
    
        int c = MgGetChar( &reader );
        if( c == '\t' )
            return reader.cursor;
        MgUnGetChar( &reader, c );
    
        for( int ii = 0; ii < 4; ++ii )
        {
            int d = MgGetChar( &reader );
            if( d != ' ' )
                return 0;
        }
        return reader.cursor;
    }
    
    MgElement* ParseIndentedCode(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        MgLine* firstLine = GetLine(ioLineRange);
        MgLine* lastLine = firstLine;
    
        MgLine* line = firstLine;
        for(;;)
        {
            if( !line )
                break;
    
            char const* lineStart = CheckIndentedCodeLine(line);
            if( !lineStart )
            {
                if( line == firstLine )
                    return 0; // this isn't a code block at all!
                else
                    break; // end of the code block
            }
    
            // if the line that starts the paragraph looks like a
            // literate scrap introduction `<< foo >>=`, then end
            // this code block so we can start a new one.
            if( line != firstLine
                && CheckLiterateScrapIntroductionLine(context, inputFile, lineStart, line) )
            {
                break;
            }
    
            // we are starting a paragraph within the code block
    
            // continue consuming lines until we see an empty line
            while( line && !IsBlankLine(line) )
            {
                lineStart = CheckIndentedCodeLine(line);
                if(!lineStart)
                    break; // end of the code element...
    
                line->text.begin = lineStart;
                lastLine = line;
                line = GetLine( ioLineRange );
            }
    
            // continue consuming lines until we see a non-empty line
            while( line && IsBlankLine(line) )
            {
                line = GetLine(ioLineRange);
            }
        }
    
        LineRange innerRange = Snip( firstLine, lastLine, ioLineRange );
    
        return ParseCodeBlockBody(
            context,
            inputFile,
            innerRange,
            0, 0 ); // no way to pass in a language name
    }
    
#line 981 "source/parse-block.md"
    char const* CheckBracketedCodeLine(
        MgLine* line,
        char    c )
    {
        MgReader reader;
        InitializeLineReader( &reader, line );
    
        for( int ii = 0; ii < 3; ++ii )
        {
            int d = MgGetChar( &reader );
            if( d != c )
                return 0;
        }
        return reader.cursor;
    }
    
    MgElement* ParseBracketedCode(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange,
        char c )
    {
        MgLine* openLine = GetLine( ioLineRange );
        char const* startLang = CheckBracketedCodeLine( openLine, c );
        if( !startLang )
            return 0;
        openLine->text.begin = startLang;
    
        MgLine* firstLine = 0;
        MgLine* lastLine = 0;
    
        for(;;)
        {
            MgLine* line = GetLine( ioLineRange );
            if( !line )
                break; // \todo: this would make an un-terminated code block...
            char const* end = CheckBracketedCodeLine( line, c );
            if( end )
            {
                // \todo: what if there is text after the backticks?
                // for now, we process it as an other text...
                line->text.begin = end;
    
                break;
            }
    
            if( !firstLine )
                firstLine = line;
            lastLine = line;
        }
    
        // the span *might* be empty... just in case
        LineRange innerRange = { 0, 0 };
        if( lastLine )
            innerRange = Snip( firstLine, lastLine, ioLineRange );
    
        // `openLine` begin/end gives us the language marker, if any
        return ParseCodeBlockBody(
            context,
            inputFile,
            innerRange,
            openLine->text.begin,
            openLine->text.end );
    }
    
    MgElement* ParseBracketedCode_Backtick(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        return ParseBracketedCode( context, inputFile, ioLineRange, '`' );
    }
    
    MgElement* ParseBracketedCode_Tilde(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        return ParseBracketedCode( context, inputFile, ioLineRange, '~' );
    }
    
#line 1066 "source/parse-block.md"
    MgBool CheckLiterateScrapIntroductionLine(
        MgContext*      context,
        MgInputFile*    inputFile,
        const char*     lineStart,
        MgLine*         line )
    {
        MgString text = MgMakeString(lineStart, line->text.end);
        MgScrapKind scrapKind = kScrapKind_Unknown;
        char const* scrapIdBegin    = 0;
        char const* scrapIdEnd      = 0;
        char const* scrapNameBegin  = 0;
        char const* scrapNameEnd    = 0;
    
        return ParseLiterateScrapIntroduction(
            context, inputFile, text,
            &scrapKind,
            &scrapIdBegin,      &scrapIdEnd,
            &scrapNameBegin,    &scrapNameEnd );
    }
    
    MgElement* ParseCodeBlockBody(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange       inLineRange,
        char const*     langBegin,
        char const*     langEnd )
    {
        // check if first line looks like a
        // literate scrap introduction
        LineRange lineRange = inLineRange;
        MgLine* firstLine = GetLine( &lineRange );
    
        MgScrapKind scrapKind = kScrapKind_Unknown;
        char const* scrapIdBegin    = 0;
        char const* scrapIdEnd      = 0;
        char const* scrapNameBegin  = 0;
        char const* scrapNameEnd    = 0;
        if( ParseLiterateScrapIntroduction(
            context, inputFile, firstLine->text,
            &scrapKind,
            &scrapIdBegin,      &scrapIdEnd,
            &scrapNameBegin,    &scrapNameEnd ) )
        {
            firstLine = firstLine + 1;
        }
        else
        {
            UnGetLine( &lineRange, firstLine );
        }
    
        MgElement* firstChild = ReadSpansInRange( context, inputFile, lineRange, kMgSpanFlags_CodeBlock );
        MgElement* codeBlock = MgCreateParentElement(
            kMgElementKind_CodeBlock,
            firstChild );
    
        if( langBegin != langEnd )
        {
            MgAddAttribute( codeBlock, "class", MgMakeString( langBegin, langEnd ) );
        }
    
        MgElement* element = codeBlock;
        if( scrapIdBegin != 0 )
        {
            MgString scrapID = { scrapIdBegin, scrapIdEnd };
            MgString scrapName = { scrapNameBegin, scrapNameEnd };
            MgScrapFileGroup* scrapGroup = MgFindOrCreateScrapGroup(
                context,
                scrapKind,
                scrapID,
                inputFile );
            if( scrapName.begin != scrapName.end )
            {
                // \todo: check that we are the first!!!
                scrapGroup->nameGroup->name = MgReadSpanElements(context, inputFile, firstLine, scrapName, kMgSpanFlags_Default);
            }
    
            MgScrap* scrap = (MgScrap*) malloc(sizeof(MgScrap));
            scrap->fileGroup = scrapGroup;
            scrap->sourceLoc = MgGetSourceLoc( inputFile, firstLine, firstLine->text.begin );
            scrap->body = codeBlock;
            scrap->next = 0;
                
            MgAddScrapToFileGroup( scrapGroup, scrap );
    
            element = MgCreateParentElement(
                kMgElementKind_ScrapDef,
                element );
    
            MgAttribute* attr = MgAddCustomAttribute( element, "$scrap" );
            attr->scrap = scrap;
        }
    
    
        return element;
    }
    
#line 1164 "source/parse-block.md"
    MgBool ParseLiterateScrapIntroduction(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgString        text,
        MgScrapKind*    outScrapKind,
        char const**    outScrapIdBegin,
        char const**    outScrapIdEnd,
        char const**    outScrapNameBegin,
        char const**    outScrapNameEnd )
    {
        MgReader reader;
        MgInitializeStringReader( &reader, text );
    
        // Allow scrap introduction to start with
        // comment and whitespace:
        int c = MgGetChar( &reader );
        switch( c )
        {
        default:
            MgUnGetChar( &reader, c );
            break;
    
        case '/':
            {
                int d = MgGetChar( &reader );
                if( d != '/' )
                {
                    MgUnGetChar( &reader, d );
                    MgUnGetChar( &reader, c );
                    break;
                }
    
                SkipWhiteSpace( &reader );
                break;
            }
    
        // TODO: C-style "/* */" comments
        }
    
        if( MgGetChar(&reader) != '<' )
            return MG_FALSE;
        if( MgGetChar(&reader) != '<' )
            return MG_FALSE;
    
        // look for a possible scrap kind marker
        char const* savedCursor = reader.cursor;
        char const* scrapKindBegin = savedCursor;
        char const* scrapKindEnd = 0;
        for( ;; )
        {
            int c = MgGetChar( &reader );
            if( c == ':' )
            {
                scrapKindEnd = reader.cursor-1;
                break;
            }
    
            if( !isalpha(c) )
                break;
        }
    
        MgScrapKind scrapKind = kScrapKind_Unknown;
        if( scrapKindEnd )
        {
            // look for a kind
            MgString scrapKindName = { scrapKindBegin, scrapKindEnd };
            if( MgStringsAreEqual(scrapKindName, CString("file")) )
            {
                scrapKind = kScrapKind_OutputFile;
            }
            else if( MgStringsAreEqual(scrapKindName, CString("global")) )
            {
                scrapKind = kScrapKind_GlobalMacro;
            }
            else if( MgStringsAreEqual(scrapKindName, CString("local")) )
            {
                scrapKind = kScrapKind_LocalMacro;
            }
            else
            {
                fprintf(stderr, "unknown scrap kind: \"");
                fwrite(scrapKindName.begin, 1, scrapKindName.end - scrapKindName.begin, stderr);
                fprintf(stderr, "\"\n");
            }
        }
        else
        {
            reader.cursor = savedCursor;
        }
    
        char const* scrapIdBegin = reader.cursor;
    
        for(;;)
        {
            int c = MgGetChar(&reader);
            if( c == -1 ) break;
    
            if( c == '|' )
            {
                MgUnGetChar(&reader, c);
                break;
            }
    
            if( c == '>' )
            {
                int d = MgGetChar(&reader);
                MgUnGetChar(&reader, d );
                if( d == '>' )
                {
                    MgUnGetChar(&reader, c);
                    break;
                }
            }
        }
    
        char const* scrapIdEnd = reader.cursor;
        char const* scrapNameBegin = 0;
        char const* scrapNameEnd = 0;
    
        int pipe = MgGetChar(&reader);
        if( pipe != '|' )
        {
            MgUnGetChar(&reader, pipe);
        }
        else
        {
            scrapNameBegin = reader.cursor;
            for(;;)
            {
                int c = MgGetChar(&reader);
                if( c == -1 ) break;
                if( c == '>' )
                {
                    int d = MgGetChar(&reader);
                    MgUnGetChar(&reader, d);
                    if( d == '>' )
                    {
                        MgUnGetChar(&reader, c);
                        break;
                    }
                }
            }
            scrapNameEnd = reader.cursor;
        }
    
        if( MgGetChar(&reader) != '>' )
            return MG_FALSE;
        if( MgGetChar(&reader) != '>' )
            return MG_FALSE;
    
        if( MgPeekChar(&reader) == '+' )
            MgGetChar(&reader);
        if( MgGetChar(&reader) != '=' )
            return MG_FALSE;
    
        SkipWhiteSpace( &reader );
        if( MgGetChar(&reader) != -1 )
            return MG_FALSE;
    
        TrimLeadingSpace(&scrapIdBegin, scrapIdEnd);
        TrimTrailingSpace(scrapIdBegin, &scrapIdEnd);
    
        TrimLeadingSpace(&scrapNameBegin, scrapNameEnd);
        TrimTrailingSpace(scrapNameBegin, &scrapNameEnd);
    
        *outScrapKind       = scrapKind;
        *outScrapIdBegin    = scrapIdBegin;
        *outScrapIdEnd      = scrapIdEnd;
        *outScrapNameBegin  = scrapNameBegin;
        *outScrapNameEnd    = scrapNameEnd;
    
        return MG_TRUE;
    }
    
#line 1360 "source/parse-block.md"
    MgElement* ParseHorizontalRule(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange*      ioLineRange,
        char            c )
    {
        MgLine* firstLine = GetLine(ioLineRange);
    
        MgReader reader;
        InitializeLineReader(&reader, firstLine);
    
        int count = 0;
        for(;;)
        {
            int d = MgGetChar(&reader);
    
            if( d == -1 )
                break;
    
            if( d == c )
            {
                ++count;
            }
            else if( isspace(d) )
            {}
            else
            {
                return 0;
            }
        }
    
        if( count < 3 )
            return 0;
    
        Snip( firstLine, firstLine, ioLineRange );
    
        return MgCreateLeafElement(
            kMgElementKind_HorizontalRule,
            MgMakeString(NULL, NULL) );
    }
    
    MgElement* ParseHorizontalRule_Hypen(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        return ParseHorizontalRule( context, inputFile, ioLineRange, '-' );
    }
    
    MgElement* ParseHorizontalRule_Asterisk(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        return ParseHorizontalRule( context, inputFile, ioLineRange, '*' );
    }
    
    MgElement* ParseHorizontalRule_Underscore(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        return ParseHorizontalRule( context, inputFile, ioLineRange, '_' );
    }
    
#line 1448 "source/parse-block.md"
    MgBool ParseLinkDefinitionTitle(
        MgReader*   reader,
        char const**    outTitleBegin,
        char const**    outTitleEnd )
    {
        int openTitle = MgGetChar(reader);
        int closeTitle;
        switch(openTitle)
        {
        default:
            return MG_FALSE;
        case '"':
        case '\'':
            closeTitle = openTitle;
            break;
        case '(':
            closeTitle = ')';
            break;
        }
    
        char const* titleBegin = reader->cursor;
        char const* titleEnd = MgFindMatching(reader, closeTitle, 1);
        if( !titleEnd )
            return MG_FALSE;
    
        // \todo: enforce that we find end of line?
    
        *outTitleBegin = titleBegin;
        *outTitleEnd = titleEnd;
        return MG_TRUE;
    }
    
    MgElement* ParseLinkDefinition(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        MgString id;
        MgLine* firstLine = GetLine(ioLineRange);
    
        MgReader reader;
        InitializeLineReader(&reader, firstLine);
    
        // skip up to three spaces
        for(int ii = 0; ii < 3; ++ii)
        {
            int c = MgGetChar(&reader);
            if( c != ' ' )
            {
                MgUnGetChar(&reader, c);
                break;
            }
        }
    
        // expect a '['
        int openBrace = MgGetChar(&reader);
        if( openBrace != '[' )
            return 0;
    
        id = MgFindMatchingString(&reader, ']', 1);
        if( !id.end )
            return 0;
    
        // expect a ':'
        int colon = MgGetChar(&reader);
        if( colon != ':' )
            return 0;
    
        // skip any white space
        SkipWhiteSpace(&reader);
    
        // allow one '<' before the link
        int leftAngle = MgGetChar(&reader);
        if( leftAngle != '<' )
        {
            MgUnGetChar(&reader, leftAngle);
        }
    
        // read the URL (assume it continues until white-space)
        char const* urlBegin = reader.cursor;
        char const* urlEnd = reader.cursor;
        for(;;)
        {
            int c = MgGetChar( &reader );
            if( c == -1 || c == '>' || isspace(c) )
            {
                MgUnGetChar(&reader, c);
                break;
            }
            urlEnd = reader.cursor;
        }
    
        // expect an opening '<' to be matched by '>'
        if( leftAngle == '<' )
        {
            int rightAngle = MgGetChar( &reader );
            if( rightAngle != '>' )
                return 0;
        }
    
        // \todo: should we check that the URL is non-empty?
    
        SkipWhiteSpace(&reader);
    
        // now look for the title
        char const* titleBegin = 0;
        char const* titleEnd = 0;
        if( !MgAtEnd(&reader) )
        {
            ParseLinkDefinitionTitle(
                &reader,
                &titleBegin,
                &titleEnd );
        }
        else
        {
            MgLine* secondLine = GetLine(ioLineRange);
            InitializeLineReader(&reader, secondLine);
            SkipWhiteSpace(&reader);
    
            if( !ParseLinkDefinitionTitle(
                &reader,
                &titleBegin,
                &titleEnd ) )
            {
                UnGetLine(ioLineRange, secondLine);
            }
        }
    
        MgReferenceLink* ref = MgFindOrCreateReferenceLink(
            inputFile,
            id );
    
        // \todo: how to handle redefinition?
        ref->url.begin = urlBegin;
        ref->url.end = urlEnd;
        ref->title.begin = titleBegin;
        ref->title.end = titleEnd;
    
        // we have to return a non-NULL element to indicate
        // a successful parse...
        return MgCreateLeafElement(
            kMgElementKind_Text,
            MgMakeString(NULL, NULL));
    }
    
#line 1601 "source/parse-block.md"
    int CountTableLinePipes(
        MgLine*   line)
    {
        MgReader reader;
        InitializeLineReader(&reader, line);
    
        int pipeCount = 0;
        for( ;; )
        {
            int c = MgGetChar(&reader);
            if( c == -1 )
                break;
            else if( c == '|' )
                ++pipeCount;
            else if( c == '\\' )
                MgGetChar(&reader);
        }
        return pipeCount;
    }
    
    MgElement* ParseTableCell(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgReader*   reader,
        MgElementKind   kind )
    {
        char const* cellBegin   = reader->cursor;
        char const* cellEnd     = cellBegin;
        for( ;; )
        {
            cellEnd = reader->cursor;
            int c = MgGetChar( reader );
            if( c == '|' || c == -1 )
            {
                MgUnGetChar(reader, c);
                break;
            }
    
            // allow escapes?
            if( c == '\\' )
            {
                int d = MgGetChar(reader);
            }
        }
    
        MgElement* firstChild = MgReadSpanElements(context, inputFile, line, MgMakeString(cellBegin, cellEnd), kMgSpanFlags_Default);
        return MgCreateParentElement(
            kind,
            firstChild );
    }
    
    MgBool ParseTableAlignments(
        MgLine* line)
    {
        MgReader reader;
        InitializeLineReader(&reader, line);
    
        int pipeCount = 0;
        int columnCount = 0;
        for( ;; )
        {
            MgBool leftJustify = MG_FALSE;
            MgBool rightJustify = MG_FALSE;
    
            if( MgPeekChar(&reader) == '|' )
            {
                MgGetChar(&reader);
                ++pipeCount;
            }
    
            if( MgPeekChar(&reader) == -1 )
                break;
    
            ++columnCount;
            if( MgPeekChar(&reader) == ':' )
            {
                MgGetChar(&reader);
                leftJustify = MG_TRUE;
            }
    
            while( MgPeekChar(&reader) == '-' )
                MgGetChar(&reader);
    
            if( MgPeekChar(&reader) == ':' )
            {
                MgGetChar(&reader);
                rightJustify = MG_TRUE;
            }
    
            int next = MgPeekChar(&reader);
            if( next != -1 && next != '|' )
                return MG_FALSE;
        }
    
        return pipeCount != 0;
    }
    
    MgElement* ParseTableRow(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         line,
        MgElementKind   cellKind )
    {
        MgReader reader;
        InitializeLineReader( &reader, line );
    
        // start reading cells
        MgElement* firstCell = 0;
        MgElement* lastCell = 0;
        for(;;)
        {
            // note that this treats a pipe at the start
            // of the line as always being a decorative pipe
            if( MgPeekChar(&reader) == '|' )
            {
                MgGetChar(&reader);
            }
    
            // end of line?
            //
            // note that this always treats a pipe at the end
            // of the line as a decorative pipe
            if( MgPeekChar(&reader) == -1 )
                break;
    
            MgElement* cell = ParseTableCell(context, inputFile, line, &reader, cellKind);
            if( lastCell )
            {
                lastCell->next = cell;
            }
            else
            {
                firstCell = cell;
            }
            lastCell = cell;
        }
    
        return MgCreateParentElement(
            kMgElementKind_TableRow,
            firstCell );
    }
    
    MgElement* ParseTable(
        MgContext*    context,
        MgInputFile*  inputFile,
        LineRange*  ioLineRange )
    {
        MgLine* headerLine      = GetLine( ioLineRange );
        MgLine* alignmentLine   = NULL;
        MgLine* firstLine       = NULL;
        MgLine* lastLine        = NULL;
        LineRange innerRange;
    
        int headerPipes = CountTableLinePipes( headerLine );
        if( headerPipes == 0 )
            return 0;
    
        // \todo: how to do table cell alignment?
        alignmentLine = GetLine( ioLineRange );
        if( !alignmentLine || !ParseTableAlignments(alignmentLine) )
            return 0;
    
        // find successive lines that might be table rows
        for(;;)
        {
            MgLine* line = GetLine( ioLineRange );
            if( !line )
                break;
    
            int linePipes = CountTableLinePipes( line );
            if( linePipes == 0 )
            {
                UnGetLine(ioLineRange, line);
                break;
            }
            if( !firstLine )
                firstLine = line;
            lastLine = line;
        }
        // we need at least one line
        if( !firstLine )
            return 0;
    
        innerRange = Snip( firstLine, lastLine, ioLineRange );
    
        MgElement* firstRow = ParseTableRow(context, inputFile, headerLine, kMgElementKind_TableHeader);
        MgElement* lastRow = firstRow;
        for( MgLine* line = innerRange.begin; line != innerRange.end; ++line )
        {
            MgElement* row = ParseTableRow(context, inputFile, line, kMgElementKind_TableCell);
            lastRow->next = row;
            lastRow = row;
        }
    
        return MgCreateParentElement(
            kMgElementKind_Table,
            firstRow );
    }
    
#line 1806 "source/parse-block.md"
    MgElement* ParseMetaData(
        MgContext*      context,
        MgInputFile*    inputFile,
        LineRange*      ioLineRange )
    {
        MgLine* firstLine = GetLine( ioLineRange );
        if( !firstLine )
            return NULL;
    
        MgReader reader;
        InitializeLineReader( &reader, firstLine );
    
        // meta-data line can't start with whitespace
        if( isspace(MgPeekChar(&reader)) )
            return NULL;
    
        // read until a ':'
        MgString key = MgFindMatchingString(&reader, ':', 1);
        if( !key.end )
            return NULL;
    
        MgString value = MgMakeString(reader.cursor, firstLine->text.end);
        TrimTrailingSpace(value.begin, &value.end);
        TrimLeadingSpace(&value.begin, value.end);
    
        MgElement* firstChild = MgCreateLeafElement(kMgElementKind_Text, value);
        MgElement* lastChild = firstChild;
    
        for(;;)
        {
            MgLine* line = GetLine( ioLineRange );
            if( !line )
                break;
    
            InitializeLineReader( &reader, line );
            if( !isspace(MgPeekChar(&reader)) )
            {
                UnGetLine( ioLineRange, line );
                break;
            }
    
            value = line->text;
            TrimTrailingSpace(value.begin, &value.end);
            TrimLeadingSpace(&value.begin, value.end);
    
            MgElement* child = MgCreateLeafElement(kMgElementKind_Text, value);
            lastChild->next = child;
            lastChild = child;
        }
    
        MgElement* element = MgCreateParentElement(
            kMgElementKind_MetaData,
            firstChild );
        MgAddAttribute(element, "$key", key);
        return element;
    }
    
    
    /*
    Like `ParseBlockElements`, but only handles meta-data elements, and
    not general Markdown document content.
    */
    MgElement* MgParseMetaDataElements(
        MgContext*      context,
        MgInputFile*    inputFile,
        MgLine*         beginLines,
        MgLine*         endLines )
    {
        MgElement* firstElement = NULL;
        MgElement* lastElement  = NULL;
    
        LineRange lineRange = { beginLines, endLines };
        for(;;)
        {
            SkipEmptyLines(&lineRange);
            if( lineRange.begin == lineRange.end )
                break;
    
            LineRange savedLineRange = lineRange;
            MgElement* element = ParseMetaData(context, inputFile, &lineRange);
            if( !element )
            {
                fprintf(stderr, "bad metadata line\n");
                // if we failed to parse meta-data, skip a line and keep trying
                lineRange = savedLineRange;
                ++lineRange.begin;            
                continue;
            }
    
            if( lastElement )
            {
                lastElement->next = element;
            }
            else
            {
                firstElement = element;
            }
            lastElement = element;
        }
    
        return firstElement;    
    }
    
#line 2108 "source/parse-block.md"
                                       
    
#line 111 "source/parse-block.md"
    BLOCK_PARSE_FUNC(ParseBlockElement)
    {
        static const BlockParseFunc kBlockParseFuncs[] = {
            
#line 146 "source/parse-block.md"
    
#line 155 "source/parse-block.md"
    &ParseLinkDefinition,
    &ParseTable,
    &ParseBlockLevelHtml,
    &ParseBlockQuote,
    &ParseIndentedCode,
    &ParseBracketedCode_Backtick,
    &ParseBracketedCode_Tilde,
    &ParseAtxHeader,
    
#line 146 "source/parse-block.md"
                                                    
    
#line 168 "source/parse-block.md"
    &ParseHorizontalRule_Hypen,
    &ParseHorizontalRule_Asterisk,
    &ParseHorizontalRule_Underscore,
    
#line 147 "source/parse-block.md"
                                                 
    
#line 173 "source/parse-block.md"
    &ParseOrderedList,
    &ParseUnorderedList,
    
#line 148 "source/parse-block.md"
                                      
    
#line 180 "source/parse-block.md"
    &ParseSetextHeader1,
    &ParseSetextHeader2,
    
#line 149 "source/parse-block.md"
                                               
    
#line 188 "source/parse-block.md"
    &ParseDefaultParagraph,
    
#line 150 "source/parse-block.md"
                                                  
    
#line 114 "source/parse-block.md"
                                                     
        };
    
        BlockParseFunc const* funcCursor = &kBlockParseFuncs[0];
        for(;;)
        {
            
#line 131 "source/parse-block.md"
    LineRange lineRange = *ioLineRange;
    MgElement* element = (*funcCursor)( context, inputFile, &lineRange );
    
#line 120 "source/parse-block.md"
                                                                  
            
#line 137 "source/parse-block.md"
    if( element )
    {
        *ioLineRange = lineRange;            
        return element;
    }
    
#line 121 "source/parse-block.md"
                                                                             
            ++funcCursor;
        }
    }
    
#line 2109 "source/parse-block.md"
                                    
    
#line 157 "source/main.md"
                           
    
#line 7 "source/writer.md"
    typedef struct MgWriterT MgWriter;
    
#line 12 "source/writer.md"
    typedef void (*MgPutCharFunc)( MgWriter*, int );
    
#line 25 "source/writer.md"
    struct MgWriterT
    {
        MgPutCharFunc   putCharFunc;
        void*           userData;
    };
    
#line 34 "source/writer.md"
    void MgPutChar(
        MgWriter*   writer,
        int         value )
    {
        writer->putCharFunc( writer, value );
    }
    
#line 46 "source/writer.md"
    void MgWriteString(
        MgWriter* writer,
        MgString  string )
    {
        char const* cursor = string.begin;
        while( cursor != string.end )
            MgPutChar( writer, *cursor++ );
    }
    
#line 58 "source/writer.md"
    void MgWriteCString(
        MgWriter*   writer,
        char const* text)
    {
        char const* cursor = text;
        while( *cursor )
            MgPutChar( writer, *cursor++ );
    }
    
#line 75 "source/writer.md"
    void MemoryWriter_PutChar(
        MgWriter* writer,
        int     value )
    {
        char* cursor = (char*) writer->userData;
        *cursor++ = (char) value;
        writer->userData = cursor;
    }
    
#line 87 "source/writer.md"
    void MgInitializeMemoryWriter(
        MgWriter*   writer,
        void*       data )
    {
        writer->putCharFunc = &MemoryWriter_PutChar;
        writer->userData    = data;
    }
    
#line 105 "source/writer.md"
    void CountingWriter_PutChar(
        MgWriter* writer,
        int     value )
    {
        int* counter = (int*) writer->userData;
        ++(*counter);
    }
    
#line 117 "source/writer.md"
    void MgInitializeCountingWriter(
        MgWriter* writer,
        int*    counter )
    {
        writer->putCharFunc = &CountingWriter_PutChar;
        writer->userData = counter;
        *counter = 0;
    }
    
#line 158 "source/main.md"
                          
    
#line 8 "source/export.md"
    MgAttribute* MgFindAttribute(
        MgElement*  pp,
        char const* id )
    {
        MgString idString = { id, id + strlen(id) };
        for(MgAttribute* attr = pp->firstAttr; attr; attr = attr->next)
        {
            if( MgStringsAreEqual(attr->id, idString) )
                return attr;
        }
        return 0;
    }
    
#line 27 "source/export.md"
    MgBool TextIsSameAsFileOnDisk(
        MgString  text,
        char const* filePath)
    {
        MgReader reader;
        FILE* file = fopen(filePath, "rb");
        if( !file )
            return MG_FALSE;
    
        MgInitializeStringReader(&reader, text);
    
        for(;;)
        {
            int c = fgetc(file);
            int d = MgGetChar(&reader);
            if( c != d )
            {
                return MG_FALSE;
            }
    
            if( c == -1 )
                return MG_TRUE;
        }
    }
    
#line 58 "source/export.md"
    void MgWriteTextToFile(
        MgString      text,
        char const* filePath)
    {
        if( TextIsSameAsFileOnDisk(text, filePath) )
        {
            return;
        }
    
        FILE* file = fopen(filePath, "wb");
        if( !file )
        {
            fprintf(stderr, "Failed to open \"%s\" for writing\n", filePath);
            return;
        }
    
        fwrite(text.begin, 1, text.end - text.begin, file);
    
        fclose(file);
    }
    
#line 159 "source/main.md"
                          
    
#line 5 "source/export-code.md"
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
    
        char const* cc = inputFile->path;
        for(;;)
        {
            int c = *cc++;
            if( !c ) break;
    
            switch(c)
            {
            case '\\':
                MgPutChar(writer, '/');
                break;
    
            // TODO: other characters that might need escaping?
    
            default:
                MgPutChar(writer, c);
                break;
            }
        }
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
    
#line 160 "source/main.md"
                               
    
#line 5 "source/export-html.md"
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
    
#line 161 "source/main.md"
                               
    
#line 5 "source/input.md"
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
    
        LineRange range;
        range.begin = inputFile->beginLines;
        range.end   = inputFile->endLines;
        MgElement* firstElement = ParseBlockElementsInRange(
            context,
            inputFile,
            range );
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
    
#line 162 "source/main.md"
                         
    
#line 6 "source/options.md"
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
    
#line 163 "source/main.md"
                           
    
#line 126 "source/main.md"
                   
    
#line 127 "source/main.md"
               
    
#line 7 "source/main.md"
    int main(
        int     argc,
        char**  argv )
    {
        
#line 24 "source/main.md"
    MgContext context;
    memset(&context, 0, sizeof(context));
    
#line 11 "source/main.md"
                      
        
#line 34 "source/main.md"
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
    
#line 12 "source/main.md"
                         
        
#line 56 "source/main.md"
    
#line 62 "source/main.md"
    if( options.metaDataFilePath )
    {
        MgAddMetaDataFile( &context, options.metaDataFilePath );
    }
    
#line 56 "source/main.md"
                                      
    
#line 71 "source/main.md"
    for( int ii = 0; ii < argc; ++ii )
    {
        char const* path = argv[ii];
        
#line 80 "source/main.md"
    if( !MgAddInputFilePath( &context, path ) )
    {
        exit(1);
    }
    
#line 74 "source/main.md"
                                           
    }
    
#line 57 "source/main.md"
                                 
    
#line 13 "source/main.md"
                       
        
#line 91 "source/main.md"
    
#line 109 "source/main.md"
    for( MgScrapNameGroup* group = context.firstScrapNameGroup; group; group = group->next )
    {
        if( group->kind != kScrapKind_OutputFile )
            continue;
    
        MgWriteCodeFile( &context, group );
    }
    
#line 91 "source/main.md"
                               
    
#line 99 "source/main.md"
    for( MgInputFile* file = context.firstInputFile; file; file = file->next )
    {
        MgWriteDocFile( &context, file );
    }
    
#line 92 "source/main.md"
                                        
    
#line 14 "source/main.md"
                         
        return 0;
    }
    
#line 128 "source/main.md"
                       
    