Mangle: A Literate Programming Tool
===================================

Mangle is a tool for literate programming with Markdown and C family programming languages. It processes Markdown files into HTML files and also exports selected code blocks to code files.

The goals of Mangle include:

 * Require minimal effort to adopt.
 * Allow literate programs to be as readable as possible in their original (text) form.
 * Make literate programs pleasant to read when viewed as Markdown files on GitHub.

Mangle is still a work in progress, so stability is not currently its strong suit.


Getting Started
---------------

In order to use Mangle in a project, you can just copy the `mangle.c` file as well as `mangle.sh` and/or `mangle.bat` into your source tree.

In order to generate source and documentation files for a project, simply pass all of the literate source files to `mangle.bat` (for Windows) or `mangle.sh` (for everything else):

    mangle.sh *.md

This will generate one `*.html` file from each `*.md` input, as well as any code files specified within the literate source.

The script will try to build an exectable from `mangle.c` the first time you run it (or when `mangle.c` changes), and re-use it thereafter.
If for some reason the script isn't working for you, you could always just pass `mangle.c` to your favorite compiler to make an executable of your own.

Syntax
------

The general syntax of Mangle is that of [Markdown][], with an attempt to include the more important features of [Github Flavored Markdown][GFM] (there are also a few features that come from [Discount][]).

  [Markdown]:   http://daringfireball.net/projects/markdown/                "Markdown"
  [GFM]:        https://help.github.com/articles/github-flavored-markdown   "GitHub Flavored Markdown"
  [Discount]:   http://www.pell.portland.or.us/~orc/Code/discount/          "Discount"

Mangle then adds a few new pieces of syntax inspired by [noweb][] for defining a literate program within the code blocks in one or more Markdown documents.

  [noweb]:      http://www.cs.tufts.edu/~nr/noweb/                          "Noweb"

The [`hello-world`](examples/hello-world/hello-world.md) example includes a brief introduction to using Mangle.

License
--------

Mangle is made available under the [MIT License](http://opensource.org/licenses/MIT).

    <<global:license>>=
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
