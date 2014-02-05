# Mangle: A Literate Programming Tool

Mangle is a tool for literate programming with Markdown and C family programming languages. It processes Markdown files into HTML files and then exports selected code blocks to code files.

The goal of Mangle is to allow literate programs to be as readable as possible in their original (text) form, as well as when browsed on GitHub. As such, the syntax for defining the liteate program "scraps" tries to be as minimal as possible.

## Building

A fairly simple Makefile and a Visual Studio 2013 project are included, but in practice you should just be able to pass all the .cpp files in the source/ directory to your favorite C++ compiler:

    c++ -o mangle *.cpp

## Running

In order to generate source and documentation files for a project, simply pass all of the literate programming source files to mangle:

    mangle *.md

You can also pass files one at a time, so long as you don't have any "global scraps" (those that accumulate across files).

## Syntax

The general syntax of Mangle is that of [Markdown][], with an attempt to include the more important features of [Github Flavored Markdown][GFM] (there are also a few features that come from [Discount][]).

  [Markdown]:   http://daringfireball.net/projects/markdown/                "Markdown"
  [GFM]:        https://help.github.com/articles/github-flavored-markdown   "GitHub Flavored Markdown"
  [Discount]:   http://www.pell.portland.or.us/~orc/Code/discount/          "Discount"

Mangle then adds a few new pieces of syntax inspired by [noweb][] for defining a literate program within the code blocks in one or more Markdown documents.

  [noweb]:      http://www.cs.tufts.edu/~nr/noweb/                          "Noweb"

### Scrap Definitions

By default, Markdown code blocks are *not* written to any output code file:

    printf("This line won't be in the output\n");

In order for code to be part of the output program, it must be part of a named *scrap*. A scrap definition is any code block where the first line of code starts with `` << `` and ends with `` >>= ``:

    <<simple-scrap>>=
    printf("Hello, World!\n");

The preceding example defines a scrap named `simple-scrap`, which includes one line of C code.

### Scrap References, and Formatted Names

A named scrap can be referenced by another scrap, to insert its definition in place:

    <<print-stuff | output greetings using `printf`>>=
    printf("***\n");
    <<simple-scrap>>

The scrap `print-stuff` includes a reference to `simple-scrap`, simply by giving the scrap name between `` << `` and `` >> ``. This scrap also shows another feature: we can define a "pretty" name for a scrap, in addition to the internal identifier, by separating them with `|`. The identifier on the left is used when referring to the scrap within the literate program, but the Markdown text on the right will be used when representing the scrap in the formatted output.

### Extending a Scrap Definition

A single scrap name may be defined at multiple places, and its text will be the concatenation of all these definitions. Thus we can extend the ``simple-scrap`` scrap as follows:

    <<simple-scrap | a simple scrap>>=
    printf("It's-a Me!\n");

Note that we can also add a "pretty" name to a previously-declared scrap (although only one definition site should include a pretty name). Note also that a scrap can be referenced before it is extended (or even before it it defined). A reference always includes the full text of the scrap (all definitions), regardless of the order of definitions.

### Scrap Kinds

When defining or extending a scrap, we can also apply a "kind" to it:

    <<global:print-stuff>>=
    printf("***\n");

In this case, we are saying that the `print-stuff` scrap is a `global` scrap, which means that it can be defined and extended across multiple input Markdown files, and the definitions across all the files will be combined to yield the scrap text (in the order the files were specified on the command line).

The default scrap kind is `local`, which means that definitions will be grouped together on a file-by-file basis, rather than globally, so that each file can re-use some common scrap names.

It is an error to declare the same scrap name with different kinds; all definition sites must either match, or not specify a kind. Users are encouraged to apply a naming scheme to avoid this problem - e.g., by prefixing all global scrap names with `g` (e.g., `g-print-stuff`).

### Output Files

Just defining scraps *still* doesn't lead to any code files being output. In order to tell Mangle to write a code file to disk, you need to use a scrap definition with the `file` kind:

    <<file:hello.c | example program `hello.c`>>=
    #include <stdio.h>

    void main( int argc, char** argv )
    {
        <<print-stuff>>
        return 0;
    }

When a scrap has the `file` kind, Mangle uses its internal name as the file name to output. When Mangle is run on this file (`README.md`), it will output the body of the above scrap (and the scraps it references) to `hello.c`.

The expected output in this case looks like:

```

#line 75 "README.md"

#line 83 "README.md"
    #include <stdio.h>

void main( int argc, char** argv )
{
    
#line 51 "README.md"
    printf("***\n");

#line 42 "README.md"
    printf("Hello, World!\n");

#line 61 "README.md"
    printf("It's-a Me!\n");

#line 52 "README.md"
                    

#line 70 "README.md"
    printf("***\n");

#line 87 "README.md"
                       
    return 0;
}
```

As you can see, Mangle automatically inserts `#line` directives that link the generated code file back to the Markdown source.

## License

Copyright (c) 2014 Tim Foley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
