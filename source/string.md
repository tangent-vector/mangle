Strings
=======

Return true iff the given `string` is empty.

    <<global:string definitions>>=
    static MgBool MgIsEmptyString(
        MgString string)
    {
        return string.begin == string.end;
    }

Make a string representing the characters between the pointers `begin` and
`end` (exclusive).

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

Make an empty string (both pointers NULL).

    <<string definitions>>=
    MgString MgMakeEmptyString()
    {
        return MgMakeString(MG_NULL, MG_NULL);
    }

Make an `MgString` from a null-terminated string. That is, point `end` at
the terminating null byte.

    <<string definitions>>=
    MgString MgTerminatedString(
        char const* begin)
    {
        return MgMakeString(begin, begin + strlen(begin));
    }

Return true iff the `left` and `right` strings are equal, modulo case
(for standard ASCII characters).

    <<string definitions>>=
    MgBool MgStringsAreEqualNoCase(
        MgString left,
        MgString right )
    {
        char const* leftCursor = left.begin;
        char const* rightCursor = right.begin;

        for(;;)
        {
            MgBool leftAtEnd = leftCursor == left.end;
            MgBool rightAtEnd = rightCursor == right.end;
            if( leftAtEnd || rightAtEnd )
                return leftAtEnd == rightAtEnd;

            char leftChar = *leftCursor++;
            char rightChar = *rightCursor++;
            if( tolower(leftChar) != tolower(rightChar) )
                return MG_FALSE;
        }
    }

Return true iff the `left` and `right` strings are equal.

    <<string definitions>>=
    MgBool MgStringsAreEqual(
        MgString left,
        MgString right )
    {
        MgReader leftReader;
        MgReader rightReader;

        MgInitializeStringReader( &leftReader, left );
        MgInitializeStringReader( &rightReader, right );

        while( !MgAtEnd(&leftReader)
            && !MgAtEnd(&rightReader) )
        {
            int c = MgGetChar(&leftReader);
            int d = MgGetChar(&rightReader);

            if( c != d )
                return MG_FALSE;
        }

        return MgAtEnd(&leftReader) == MgAtEnd(&rightReader);
    }
