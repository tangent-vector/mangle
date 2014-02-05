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
#ifndef MANGLE_EXPORT_H_INCLUDED
#define MANGLE_EXPORT_H_INCLUDED

/*
Utilities shared by both the documentation and code export paths.
*/

#include "string.h"

/*
Find an existing attribute with the given `id`.
Return NULL if not found.
*/

MgAttribute* MgFindAttribute(
    MgElement*  pp,
    char const* id );

/*
Write the string `text` to the file specified by `path`, but first check
whether there is already a file on disk with that path with exactly the same
text (in which case don't write anything). This avoids triggerring unneeded
builds for build systems that check file modification times (e.g., `make`).
*/
void MgWriteTextToFile(
    MgString    text,
    char const* path);

#endif /* MANGLE_EXPORT_H_INCLUDED */
