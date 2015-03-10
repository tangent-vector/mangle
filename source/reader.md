Reader
------

The `MgReader` type is an abstraction for reading from an `MgString`,
one character at a time.

    <<global:reader definitions>>=
    typedef struct MgReaderT
    {
        MgString    string;
        char const* cursor;
    } MgReader;

The `MgGetChar` and `MgPeekChar` functions will return `kMgEndOfFile` when
trying to read past the end of the string.

    <<reader definitions>>=
    enum
    {
        kMgEndOfFile = -1,
    };

Set up to start reading from `string`.

    <<reader definitions>>=
    void MgInitializeStringReader(
        MgReader*   reader,
        MgString          string )
    {
        reader->string  = string;
        reader->cursor  = string.begin;    
    }


Return `MG_TRUE` if reader is at end.

    <<reader definitions>>=
    MgBool MgAtEnd(
        MgReader*   reader )
    {
        return reader->cursor == reader->string.end;
    }

Get next character from the reader, or `kMgEndOfFile` if at end.

    <<reader definitions>>=
    int MgGetChar(
        MgReader*   reader )
    {
        if( MgAtEnd(reader) )
            return kMgEndOfFile;

        return *(reader->cursor++);
    }

Back up the reader by one character. The `value` passed in should match
the most recent character (or end-of-file marker) read from the reader.

    <<reader definitions>>=
    void MgUnGetChar(
        MgReader*   reader,
        int             value )
    {
        if( value == kMgEndOfFile )
            return;

        --(reader->cursor);
    }

Get the character before the current position in the reader, or
`kMgEndOfFile` if we are at the beginning of the string.

TODO: Is this function really needed? It is a bit gross to have.

    <<reader definitions>>=
    int MgGetPrecedingChar(
        MgReader*   reader )
    {
        if( reader->cursor == reader->string.begin )
            return -1;

        return *(reader->cursor - 1);
    }

Return the next character that `MgGetChar` would yield, without advancing
the cursor of the reader.

    <<reader definitions>>=
    int MgPeekChar(
        MgReader* reader )
    {
        int result = MgGetChar( reader );
        MgUnGetChar( reader, result );
        return result;
    }
