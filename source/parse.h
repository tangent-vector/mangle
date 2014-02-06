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
#ifndef MANGLE_PARSE_H_INCLUDED
#define MANGLE_PARSE_H_INCLUDED

/*
Interface for both block- and span-level Markdown parsing code.
*/

#include <mangle/mangle.h>

#include "reader.h"

/*
Type for lines that have been loaded into memory.

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
*/
struct MgLineT
{
    MgString      text;
    char const* originalBegin;
};

/*
Look ahead in the reader for an occurence of character `c`, `count` times
in a row. Return a pointer to right before the first character in the match,
and leave the cursor of the reader pointing right after the match.

If no match is found, returns NULL.
*/
char const* MgFindMatching(
    MgReader*   reader,
    char        c,
    int         count );

/*
Similar to `MgFindMatching`, except that it returns the intial cursor of the
reader as the `begin` of the result string, and the result from
`MgFindMatching` as the `end`.

Note: this means that when no match is found, the `end` of the result will
be NULL, but the `begin` won't be.
*/
MgString MgFindMatchingString(
    MgReader*   reader,
    char        c,
    int         count );

/*
Create a leaf document element, with the specified kind and text.
*/
MgElement* MgCreateLeafElement(
    MgElementKind   kind,
    MgString        text );

/*
Create a parent document element, with the specified first child in
the linked list of child elements.
*/
MgElement* MgCreateParentElement(
    MgElementKind   kind,
    MgElement*      firstChild );

/*
Allocate and add a new attribute to an existing element, with the
specified NULL-terminated `id` and value.
*/
MgAttribute* MgAddAttribute(
    MgElement*  element,
    char const* id,
    MgString    val );

/*
Allocate an add a new atttribute to an existing element, with the
specified NULL-terminated `id`, but with no value. The caller is
expected to fill in of the fields in the `union` inside the
attribute, based on the chosen ID.
*/
MgAttribute* MgAddCustomAttribute(
    MgElement*  element,
    char const* id );

/*
When encountering either a reference to or a definition of a "reference-style"
link in the document, call this function to get or create the object to represent
the link. At a definition site, fill in the `MgReferenceLink*` that was returned,
to provide information that will be used at any sites that reference it.
*/
MgReferenceLink* MgFindOrCreateReferenceLink(
    MgInputFile*    inputFile,
    MgString        id );

/*
When encountering either a reference to or a definition of a scrap,
call this function to get or create the object that represents the scrap
group with that `id` for the given `file`.

The `kind` can either be `kMgScrapKind_Unknown` if you don't care what
kind of scrap it is, or a specific scrap kind if you want to set the
scrap kind as part of retrieving it. (TODO: separate those steps)
*/
MgScrapFileGroup* MgFindOrCreateScrapGroup(
    MgContext*      context,
    MgScrapKind       kind,
    MgString        id,
    MgInputFile*    file );

/*
Add a particular scrap definition to a group of scraps with the same
ID in the same file.
*/
void MgAddScrapToFileGroup(
    MgScrapFileGroup* fileGroup,
    MgScrap*          scrap);

/*
Return line number and column information (1-based) for a location in the
given input file, represented by the given pointer into the given line.
*/
MgSourceLoc MgGetSourceLoc(
    MgInputFile*    inputFile,
    MgLine*         line,
    const char*     cursor );

/*
Parse block-level elements from Markdown source, in the range between
`beginLines` and `endLines`, which should be lines in the input file
`inputFile` loaded in `context`.

Returns the first element parsed, which is the start of a linked list
of elements (each of which may have its own child elements).
*/
MgElement* MgParseBlockElements(
    MgContext*      context,
    MgInputFile*    inputFile,
    MgLine*         beginLines,
    MgLine*         endLines );

/*
Like `MgParseBlockElements`, but only handles meta-data elements, and
not general Markdown document content.
*/
MgElement* MgParseMetaDataElements(
    MgContext*      context,
    MgInputFile*    inputFile,
    MgLine*         beginLines,
    MgLine*         endLines );

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
    MgSpanFlags     flags );

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
    MgSpanFlags     flags );

#endif /* MANGLE_PARSE_H_INCLUDED */

