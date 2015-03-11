"Hello, World!"
===============

In order to demonstrate how to use Mangle, we will develop a traditional "Hello, World" program.

Scrap Definitions
-----------------

By default, Markdown code blocks are *not* written to any output code file:

    printf("This line won't be in the output\n");

In order for code to be part of the output program, it must be part of a named *scrap*. A scrap definition is any code block where the first line of code starts with `` << `` and ends with `` >>= ``:

    <<hello-world>>=
    printf("Hello, World!\n");

The preceding example defines a scrap named `simple-scrap`, which includes one line of C code.

Scrap References
----------------

A named scrap can be referenced by another scrap, to insert its definition in place:

    <<print greeting between markers>>=
    printf("***\n");
    <<hello-world>>

The scrap `output greeting between markers` includes a reference to `hello-world`, simply by giving the scrap name between `` << `` and `` >> ``.

Extending a Scrap Definition
----------------------------

The previous scrap definition only included an opening `***` marker.
To give it another marker on the other side of the greeting, we can *extend* the scrap with another definition:

    <<print greeting between markers>>+=
    printf("***\n");

Here we've used `+=` instead of just `=` at the end of the first line, to indicate that we are extending a pre-existing scrap definition, but this is not actually required by Mangle.

A scrap can be referenced before it is extended (or event before it is defined).
A reference to a scrap always expands to the full text of the scrap (all definitions), regardless of the order of definitions.

"Pretty" Names
--------------

When defining or extending a scrap, we can give it a "pretty" name, in addition to the internal identifier.
The pretty name comes after the identifier, separated by a `|`:

    <<hello-world | print a greeting>>=
    printf("It's-a Me!\n");

Output Files
------------

Just defining scraps doesn't lead to any code files being output.
In order to tell Mangle to write a code file to disk, you need to use a scrap definition with the `file:` prefix:

    <<file:hello.c | example program `hello.c`>>=
    #include <stdio.h>

    void main( int argc, char** argv )
    {
        <<print greeting between markers>>
        return 0;
    }

When Mangle is run on this file (`hello-world.md`), it will output the body of the above scrap (and the scraps it references) to `hello.c`.

The expected output in this case looks like:

```

#line 61 "examples/hello-world/hello-world.md"
    #include <stdio.h>
    
    void main( int argc, char** argv )
    {
        
#line 26 "examples/hello-world/hello-world.md"
    printf("***\n");
    
#line 16 "examples/hello-world/hello-world.md"
    printf("Hello, World!\n");
    
#line 52 "examples/hello-world/hello-world.md"
    printf("It's-a Me!\n");
    
#line 27 "examples/hello-world/hello-world.md"
                   
    
#line 38 "examples/hello-world/hello-world.md"
    printf("***\n");
    
#line 65 "examples/hello-world/hello-world.md"
                                          
        return 0;
    }
    
```

As you can see, Mangle automatically inserts `#line` directives that link the generated code file back to the Markdown source.
