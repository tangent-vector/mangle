Data Types
==========

This section describes the main in-memory data types used by Mangle.
The parsing code generates data structures to represent one or more input files, and the export code walks over those structures to generate formatted code or documentation.

Boolean Values
--------------

We define our own Boolean type, since we are using C and can't assume C11.

    <<document type declarations>>+=
    typedef int MgBool;
    #define MG_TRUE     (1)
    #define MG_FALSE    (0)


Source Locations
----------------

The `SourceLoc` type represents a location within a source file, as a line and column number.

    <<document type declarations>>=
    typedef struct MgSourceLocT
    {
        int line;
        int col;
    } MgSourceLoc;

Both the line and column number are one-based.

Literate Code
-------------

### Scraps ###

A `Scrap` represents a single definition of a piece of literate code.

    <<document type declarations>>+=
    struct MgScrapT
    {
        <<scrap members>>
    };
    
Every scrap remembers its starting location within its source file, and keeps track of the body text of the scrap, as parsed document elements.

    <<scrap members>>=
    MgSourceLoc         sourceLoc;
    MgElement*          body;

### Kinds of Scraps ###

Every scrap name will have an associated *kind*.

    <<document type declarations>>+=
    typedef enum MgScrapKind
    {
        <<scrap kinds>>
    } MgScrapKind;

The user can specify the kind for a scrap using a prefix on its name when declaring it.

#### Unknown ####

If the parser hasn't seen an explicit kind prefix associated with a scrap yet, then it defaults to the "unknown" kind.

    <<scrap kinds>>+=
    kScrapKind_Unknown,

#### Local Macros ####

If the user never declares a kind for a given scrap name, then it will by default behave as a local macro.
The user can explicitly request this behavior with the `local:` prefix.

Note: a future version might change the default to `global:`, and possibly get rid of `local:`.
It isn't clear that this feature is worth the hassle.

    <<scrap kinds>>+=
    kScrapKind_LocalMacro,

When referencing a local scrap, the exporter will insert the body of all scrap definitions with the same name in the same input file.

#### Global Macros ###

A global scrap is declared with the `global:` prefix.

    <<scrap kinds>>+=
    kScrapKind_GlobalMacro,

When referencing a global scrap, the exporter inserts the body of all scrap definitions with the sae name across *all* input files.

#### Output Files ####

A scrap declared with the kind prefix `file:` represents an output code file.

    <<scrap kinds>>+=
    kScrapKind_OutputFile,

### File Groups ###

On a per-source-file basic, we collect together scrap definitions with the same name to make a `ScrapFileGroup`.

    <<document type declarations>>+=
    struct MgScrapFileGroupT
    {
        <<scrap file group members>>
    };

Each scrap file group records the input file that contains all the scrap definitions.

    <<scrap file group members>>+=
    MgInputFile*      inputFile;

We organize the scraps within a file group into a singly-linked list.
The scrap group holds onto a pointer to both the first and last scrap in the list, so that we can append to the end of the list in constant time.

    <<scrap members>>+=
    MgScrap*            next;

    <<scrap file group members>>+=
    MgScrap*          firstScrap;
    MgScrap*          lastScrap;

We also give each scrap a pointer to the file group that contains it.

    <<scrap members>>+=
    MgScrapFileGroup*   fileGroup;

### Name Groups ###

We further aggregate file groups into `ScrapNameGroup`s, which collect all the scrap with the same name, across all input files.

    <<document type declarations>>+=
    struct MgScrapNameGroupT
    {
        <<scrap name group members>>
    };

Every name group keeps track of the internal identifier shared by all the scraps, along with any external "pretty" name that has been set.

Note: this might change to only support a single name.

    <<scrap name group members>>+=
    MgString            id;
    MgElement*          name;

The name group also keeps track of the kind associated with the scrap, if any.

    <<scrap name group members>>+=
    MgScrapKind         kind;

Note that it is invalid to have multiple scraps, or file groups, with the same name but different kinds.
If any file declares that a scrap has `global:` or `file:` kind, then that indicates a desire to merge definitions across all files, which would be incompatible with any `local:` declaration.
We rule this scenario out explicitly in our data type, which forces us to deal with such errors during parsing.

Similarly to how scraps are organized within file groups, we put the file groups with the same name into a singly-linked list, with the name group holding pointers to both the first and last entries in the list.

    <<scrap file group members>>+=
    MgScrapFileGroup* next;

    <<scrap name group members>>+=
    MgScrapFileGroup*   firstFileGroup;
    MgScrapFileGroup*   lastFileGroup;

Every scrap file group will also hold a pointer to its parent name group.

    <<scrap file group members>>+=
    MgScrapNameGroup* nameGroup;

These parent pointers mean that a `Scrap` doesn't need to store its own name, or a pointer to the file that contains it.
To get the name of a scrap `s` we simply use `s->fileGroup->nameGrop->name`.
To get the input file that a scrap was declared in, we use `s->fileGroup->inputFile`.

TODO: the following declaration should be moved over with `Context`:

    <<scrap name group members>>+=
    MgScrapNameGroup*   next;

Input Lines
-----------

We use the `MgLine` type for lines that have been loaded into memory.

    <<document type declarations>>+=
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


Input Files
-----------

An `InputFile` represents an input file -- usually a Markdown document -- that has been read in by Mangle.

    <<document type declarations>>+=
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


Context
-------

The `Context` type holds the state of the entire Mangle session.

    <<document type declarations>>+=
    struct MgContextT
    {
        MgInputFile*        firstInputFile;         /* singly-linked list of input files */
        MgInputFile*        lastInputFile;

        MgScrapNameGroup*   firstScrapNameGroup;    /* singly-linked list of scrap name groups */
        MgScrapNameGroup*   lastScrapNameGroup;

        MgInputFile*        metaDataFile;
    };


Markdown
--------

#### Kinds of Elements ###

The kinds of elements that Mangle supports closely match HTML element types, as well as Markdown document structure.

    <<document type declarations>>+=
    typedef enum MgElementKindT
    {
        <<element kinds>>
    } MgElementKind;

Broadly, we can categorize every element as either block-level, or span-level.

    <<element kinds>>=
    <<block-level element kinds>>
    <<span-level element kinds>>

#### Simple ####

Most element kinds closely correspond to a single HTML tag, so we won't describe them in depth.

    <<block-level element kinds>>+=
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

    <<span-level element kinds>>+=
    kMgElementKind_Em,                  /* `<em>` */
    kMgElementKind_Strong,              /* `<strong>` */
    kMgElementKind_InlineCode,          /* `<code>` */

#### Headers ####

The kinds corresponding to HTML headers `<h1>` through `<h6>` are only special in that they are contiguous in the enum.

    <<block-level element kinds>>+=
    kMgElementKind_Header1,             /* `<h1>` */
    kMgElementKind_Header2,             /* `<h2>` */
    kMgElementKind_Header3,             /* `<h3>` */
    kMgElementKind_Header4,             /* `<h4>` */
    kMgElementKind_Header5,             /* `<h5>` */
    kMgElementKind_Header6,             /* `<h6>` */

#### Code Block ####

The kind for code blocks doesn't correspond to a single element, but is still fairly trivial.
Note that a code block element is *not* guaranteed to correspond to a literate scrap definition.

    <<block-level element kinds>>+=
    kMgElementKind_CodeBlock,           /* `<pre><code>` */

#### Scrap Definition ###

A scrap definition element holds the contexts of the definition as the children of the element, and stores a pointer to the corresponding `Scrap` in an attribute named `$scrap`.

    <<block-level element kinds>>+=
    kMgElementKind_ScrapDef,

#### Scrap References ####

When the user refers to a scrap by enclosing its name in `<<` and `>>`, whether in literate code or ordinary text, we create an element with two custom attributes.
The `$scrap-group` attribute holds a pointer to the `ScrapFileGroup` being referenced.
The `$resume-at` attribute holds a `SourceLoc` for the location after the reference.

    <<span-level element kinds>>+=
    kMgElementKind_ScrapRef,

#### Meta-Data ####

In order to support meta-data declarations (e.g., to specify a CSS file to use in the HTML output), we represent each meta-data declaration as an element where its children represent the value of the meta-data declaration and an attribute with named `$key` stores the corresponding key.

    <<block-level element kinds>>+=
    kMgElementKind_MetaData,

#### Inline HTML Blocks ####

Mangle doesn't currently include an HTML parser for handling inline HTML as allowed by Markdown.
Instead, we simply try our best to find blocks that correspond to inline HTML and represent them as a special element kind where the raw text is assumed to be valid HTML.

    <<block-level element kinds>>+=
    kMgElementKind_HtmlBlock,

#### Plain Text ####

In cases where we need to create an element, but it doesn't have an other special behavior, we simply create a text element.

    <<element kinds>>+=
    kMgElementKind_Text,

#### HTML Entities ####

We create elements to represent the common HTML entities, so that we can emit them differently depending on whether we are generating HTML or source code.

    <<span-level element kinds>>+=
    kMgElementKind_LessThanEntity,      /* `&lt;` */
    kMgElementKind_GreaterThanEntity,   /* `&gt;` */
    kMgElementKind_AmpersandEntity,     /* `&amp;` */

#### Newline ####

When creating span-level elements from a line, we will always end it with an explicit newline element, rather than just a `\n` character.
This allows exporters to easily do special behavior at the start/end of each line (e.g., handling indenting for code output).

    <<span-level element kinds>>+=
    kMgElementKind_NewLine,             /* `"\n"` */

#### Simple Links ####

An inline link in Markdown will be represented as a link element with an `href` attribute.

    <<span-level element kinds>>+=
    kMgElementKind_Link,                /* `<a>` with href attribute */

#### Reference Links ####

In Markdown, reference-style links allow a link like `[Foo][]` to be created with no target.
The associated target for the link can be provided elsewhere in the input with a definition like:

    [Foo] http://foo.com "Foo"

In order to handle such links, we create a `ReferenceLink` when a particular link name is first encountered (whether on a definition or use).
Subsequent definitions or uses of the same name (case-insensitive) will refer to the same structure and may fill in whatever information becomes available.

    <<document type declarations>>+=
    struct MgReferenceLinkT
    {
        MgString          id;
        MgString          url;
        MgString          title;
        MgReferenceLink*  next;
    };

In the document strucure, when we encounter a reference-style link, we create an element with a `$referenceLink` attribute that holds a pointer to the associated `ReferenceLink`.

We do not currently create elements to represent link definitions.

    <<span-level element kinds>>+=
    kMgElementKind_ReferenceLink,

### Attributes ###

An `Attribute` is used to represent auxiliary data attached to an element.
This includes both ordinary HTML attributes, as well as other kinds of auxiliary data.

    <<document type declarations>>+=
    struct MgAttributeT
    {
        <<attribute members>>
        union
        {
            <<attribute union members>>
        };
    };

Every attribute has an identifier.
For HTML attributes, this corresponds to the ordinary attribute name (e.g., `href`).
For custom attributes, the identifier will always start with `$`.

    <<attribute members>>+=
    MgString              id;

The attributes of an element are stored in a singly-linked list.

    <<attribute members>>+=
    MgAttribute*          next;

When representing an HTML attribute, the `val` field holds the text of the attribute value.

    <<attribute union members>>+=
    MgString          val;

For custom attributes, the data will be in one of the other fields of the `union`.

    <<attribute union members>>+=
    MgReferenceLink*  referenceLink;
    MgScrap*          scrap;
    MgScrapFileGroup* scrapFileGroup;
    MgSourceLoc       sourceLoc;

### Elements ###

An `Element` represents a pieces of Markdown or HTML document structure.

    <<document type declarations>>+=
    struct MgElementT
    {
        <<element members>>
    };

Each element has a `kind` that effectively tells us what the HTML tag would be, or otherwise identifies the flavor of content.

    <<element members>>+=
    MgElementKind   kind;

An element may contain some amount of direct text.
This is used, for example, to represent the text `foo` inside of an `<i>` element like `*foo*`.

    <<element members>>+=
    MgString        text;

An element may have zero or more attributes attached to it, stored as a singly-linked list.

    <<element members>>+=
    MgAttribute*    firstAttr;

Each element may have zero or more child elements, stored as a single-linked list.

    <<element members>>+=
    MgElement*      firstChild;
    MgElement*      next;






Forward Declarations
--------------------

In order to satisfy the C compiler, we occasionally need to insert a forward declaration of a type before the definition of other types that use it.

    <<global: document forward declarations>>=
    typedef struct MgAttributeT         MgAttribute;
    typedef struct MgContextT           MgContext;
    typedef struct MgElementT           MgElement;
    typedef struct MgInputFileT         MgInputFile;
    typedef struct MgLineT              MgLine;
    typedef struct MgReferenceLinkT     MgReferenceLink;
    typedef struct MgScrapT             MgScrap;
    typedef struct MgScrapFileGroupT    MgScrapFileGroup;
    typedef struct MgScrapNameGroupT    MgScrapNameGroup;

    <<global:document declarations>>=
    <<document forward declarations>>
    <<document type declarations>>
