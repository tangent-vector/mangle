This is a Setext H1
=========

This is a Setext H2
---------

# Atx H1 #


foo [bar](http://www.google.com) baz!

I get 10 times more traffic from [Google][gg] than from
[Yahoo][] or [MSN][3].

  [gg]: http://google.com/        "Google"
  [yahoo]:  http://search.yahoo.com/  "Yahoo Search"
  [3]:    http://search.msn.com/    "MSN Search"

## Atx H2 ########

### Atx H3 ##

#### Atx H4 ####

##### Atx H5 ##

###### Atx H6 ######

This is a *regular* _paragraph_, foo_bar **and** the following __ought__ to __be_ **inline* HTML:

<table>
    <tr>
        <th>Foo</th>
        <th>Bar</th>
    </tr>
    <tr>
        <td>F</td>
        <td>B</td>
    </tr>
</table>

Let's try doing a table in Markdown:

Foo | Bar
|---|-----|
| F | B

This is another regular paragraph.

> This is a blockquote with two paragraphs. Lorem ipsum dolor sit amet,
> consectetuer adipiscing elit. Aliquam hendrerit mi posuere lectus.
> Vestibulum enim wisi, viverra nec, fringilla in, laoreet vitae, risus.
> 
> Donec sit amet nisl. Aliquam semper ipsum sit amet velit. Suspendisse
> id sem consectetuer libero luctus adipiscing.

regular text

> This is a blockquote with two paragraphs. Lorem ipsum dolor sit amet,
consectetuer adipiscing elit. Aliquam hendrerit mi posuere lectus.
Vestibulum enim wisi, viverra nec, fringilla in, laoreet vitae, risus.

> Donec sit amet nisl. Aliquam semper ipsum sit amet velit. Suspendisse
id sem consectetuer libero luctus adipiscing.

regular text

> This is the first level of quoting.
>
> > This is nested blockquote.
>
> Back to the first level.

regular text

> ## This is a header.
> 
> 1.   This is the first list item.
> 2.   This is the second list item.
> 
> Here's some example code:
> 
>     return shell_exec("echo $input | $markdown_script");

*   Red
*   Green
*   Blue

is equivalent to:

+   Red
+   Green
+   Blue

and:

-   Red
-   Green
-   Blue

Ordered lists use numbers followed by periods:

1.  Bird
2.  McHale
3.  Parish

If you instead wrote the list in Markdown like this:

1.  Bird
1.  McHale
1.  Parish

or even:

3. Bird
1. McHale
8. Parish

lists can have multiple paragraphs:

1.  This is a list item with two paragraphs. Lorem ipsum dolor
    sit amet, consectetuer adipiscing elit. Aliquam hendrerit
    mi posuere lectus.

    Vestibulum enim wisi, viverra nec, fringilla in, laoreet
    vitae, risus. Donec sit amet nisl. Aliquam semper ipsum
    sit amet velit.

2.  Suspendisse id sem consectetuer libero luctus adipiscing.

or the lazy form:

*   This is a list item with two paragraphs.

    This is the second paragraph in the list item. You're
only required to indent the first line. Lorem ipsum dolor
sit amet, consectetuer adipiscing elit.

*   Another item in the same list.

nesting quotes or code inside a list item:

*   A list item with a blockquote:

    > This is a blockquote
    > inside a list item.

*   A list item with a code block:

        <code goes here>

This is a normal paragraph:

    This is a code block.

Here is an example of AppleScript:

    tell application "Foo"
        beep
    end tell


horizontal rule types:

hyphens

---

asterisks

* * *

underscores

________________

there we go...



# A Hello World Program#

```
<<print-it>>=
printf("Hello, World!\n");
```

and here's the `main` (testing referencing a section name <<print-it>>):

```
<<file: main.cpp>>=
#include <stdio.h>
int main(
    int     argc,
    char**  argv )
{

    <<print-it>>
    return 0;

}
```

    <<print-it | *print* it>>=
    printf("It's-a Me\n");

The following is bracketed code using `` std`cout `` (just testing backticks...):

```C++
std::out << "Hello, World!" << std::endl;
```

