Reader
------

An `MgReader` is an abstraction for reading from an `MgString`, one character at a time.
The main benefits it has over just doing pointer-based reading are:

 * we get an end-of-file marker when trying to read past the end of the input (rather than unpredictable results), and
 * we can "unget" characters easily to replay input

The state of a reader consist of the original string it is operating on, along with a "cursor" into that string.

    <<global:reader definitions>>=
    typedef struct MgReaderT
    {
        MgString    string;
        char const* cursor;
    } MgReader;

If we try to read past the end of the string, we will return the `kMgEndOfFile` marker.
We might eventually switch this to use the C standard library `EOF` instead.

    <<reader definitions>>=
    enum
    {
        kMgEndOfFile = -1,
    };

To start reading from a string, we simply set the cursor of the reader to the beginning of the string.

    <<reader definitions>>=
    void MgInitializeStringReader(
        MgReader*   reader,
        MgString          string )
    {
        reader->string  = string;
        reader->cursor  = string.begin;    
    }

A reader is at the end of the input when the cursor is equal to the `end` field of the original string.

    <<reader definitions>>=
    MgBool MgAtEnd(
        MgReader*   reader )
    {
        return reader->cursor == reader->string.end;
    }

When trying to read a character, we first check if we are at the end of the input, and if so we return `kMgEndOfFile`.
Otherwise, we read a character from the cursor and increment it.

    <<reader definitions>>=
    int MgGetChar(
        MgReader*   reader )
    {
        if( MgAtEnd(reader) )
            return kMgEndOfFile;

        return *(reader->cursor++);
    }

In order to "unget" a non-end-of-file character, we need only decrement the cursor, assuming that the user is un-getting the most recent character read.
If we are trying to unget an end-of-file character, then that is the result of trying to read past the end of the input, so we leave the cursor at the end.

    <<reader definitions>>=
    void MgUnGetChar(
        MgReader*   reader,
        int             value )
    {
        if( value == kMgEndOfFile )
            return;

        --(reader->cursor);
    }

The `MgUnGetChar` function could probably do with some more validation, since it doesn't catch attempts to unget the wrong value, or to unget past the beginning of the input.

Sometimes we just want to "peek" ahead in a reader, to see what the next call to `MgGetChar` would return.
The implementation is basically the same as `MgGetChar`, just without the increment of the cursor.

    <<reader definitions>>=
    int MgPeekChar(
        MgReader*   reader )
    {
        if( MgAtEnd(reader) )
            return kMgEndOfFile;

        return *(reader->cursor);
    }
