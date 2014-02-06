Meta-Data Example
=================

This is a very basic example to show how meta-data can be specified in an external file to add a custom page title or CSS style without disrupting the overall flow of the text document.

Note how the page title of the generated HTML file does not match the top-most header of the document (as it normally would), because it is overriden in the external file `meta.txt`. Note also how the HTML is styled by the local `style.css` file, whereas it would otherwise get no style applied.

A Bunch of Syntax
-----------------

In order to confirm that our style-sheet is working, here are some basic syntax tests:

>   This is a block quote.
>
>>  This is a nested quote
>
>       /* and this is code inside a block quote */

---

Here's a table:

| Apples | Oranges |
|--------|---------|
|  Yes   |  Maybe  |

---

Here's some literate code, just for kicks:

    <<factorial | `factorial` function>>=
    int factorial( int n )
    {
        int result = 1;
        while( n > 1 )
            result *= n--;
        return result;
    }
