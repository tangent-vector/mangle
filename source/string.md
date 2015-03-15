Strings
=======

Representation
--------------

Because we want to avoid memory allocation as much as possible, we represent strings as two pointers into an existing allocation.
The `begin` field points to the first byte in the range, and the `end` pointer points after the last byte.

    <<global:string declarations>>=
    typedef struct MgStringT
    {
        char const* begin;
        char const* end;
    } MgString;

Empty Strings
-------------

An empty string is one where `begin` and `end` are equal.

    <<global:string definitions>>=
    static MgBool MgIsEmptyString(
        MgString string)
    {
        return string.begin == string.end;
    }

A canonical empty string is one where both pointers are `NULL`.

    <<string definitions>>=
    MgString MgMakeEmptyString()
    {
        return MgMakeString(NULL, NULL);
    }

Creating
--------

For convenience, we provide a "constructor"  to make a string from `begin` and `end` pointers.

    <<string declarations>>+=
    MgString MgMakeString( char const* begin, char const* end );

    <<string definitions>>=
    MgString MgMakeString(
        char const* begin,
        char const* end)
    {
        MgString result;
        result.begin    = begin;
        result.end      = end;
        return result;
    }

We can also create an `MgString` from a traditional null-terminated C string.
We simply set `end` to point at the terminating null byte.

    <<string definitions>>=
    MgString MgTerminatedString(
        char const* begin)
    {
        return MgMakeString(begin, begin + strlen(begin));
    }

Comparison
----------

### Default ###

In order to compare two strings for equality, we will loop over and compare their characters one-by-one, making sure to terminate if we reach the end of either string.

    <<string definitions>>=
    MgBool MgStringsAreEqual(
        MgString left,
        MgString right )
    {
        char const* leftCursor = left.begin;
        char const* rightCursor = right.begin;

        for(;;)
        {
            <<check for end of strings>>
            <<read characters to compare>>
            <<compare characters for equality>>
        }
    }

If we reach the end of either string, we are done.
The strings are equal if they both reached the end at the same time.

    <<check for end of strings>>=
    MgBool leftAtEnd = leftCursor == left.end;
    MgBool rightAtEnd = rightCursor == right.end;
    if( leftAtEnd || rightAtEnd )
        return leftAtEnd == rightAtEnd;

If neither string is at the end, we read a character from each, and then compare them.

    <<read characters to compare>>=
    char leftChar = *leftCursor++;
    char rightChar = *rightCursor++;

    <<compare characters for equality>>=
    if( leftChar != rightChar )
        return MG_FALSE;

### Case-Insensitive ###

We also need a case-insensitive comparison, which follows the same overall logic, just with a case-insensitive comparison of characters.

    <<string definitions>>=
    MgBool MgStringsAreEqualNoCase(
        MgString left,
        MgString right )
    {
        char const* leftCursor = left.begin;
        char const* rightCursor = right.begin;

        for(;;)
        {
            <<check for end of strings>>
            <<read characters to compare>>
            <<compare characters for case-insensitive equality>>
        }
    }

    <<compare characters for case-insensitive equality>>=
    if( tolower(leftChar) != tolower(rightChar) )
        return MG_FALSE;
