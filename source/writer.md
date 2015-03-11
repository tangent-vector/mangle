Writer
======

The `MgWriter` type provides an interface for writing characters.
Implementations of this interface are provided for writing to memory buffers,
as well as a "writer" that simply counts characters.
Other writers would be easy to define; they just require an `MgPutChar` callback.

Interface
---------

    <<global:writer definitions>>=
    typedef struct MgWriterT MgWriter;

    typedef void (*MgPutCharFunc)( MgWriter*, int );

    struct MgWriterT
    {
        MgPutCharFunc   putCharFunc;
        void*           userData;
    };

Set up a writer that calls the `putCharFunc` callback for each output
character, passing the specified `userData` as an extra argument.

    <<writer definitions>>=
    void MgInitializeWriter(
        MgWriter*     writer,
        MgPutCharFunc putCharFunc,
        void*       userData )
    {
        writer->putCharFunc = putCharFunc;
        writer->userData    = userData;
    }

Write a single character to a writer.

Note: this function takes an `int` paramter, but only `char` values
should be passed in (all other values will be cast to `char`).

    <<writer definitions>>=
    void MgPutChar(
        MgWriter*     writer,
        int         value )
    {
        writer->putCharFunc( writer, value );
    }

Write all characters in range starting at pointer `begin` up to
the pointer `end` (not inclusive).

    <<writer definitions>>=
    void MgWriteRange(
        MgWriter* writer,
        char const* begin,
        char const* end)
    {
        char const* cursor = begin;
        while( cursor != end )
            MgPutChar( writer, *cursor++ );
    }

Write all characters in `string`.

    <<writer definitions>>=
    void MgWriteString(
        MgWriter* writer,
        MgString  string )
    {
        MgWriteRange( writer, string.begin, string.end );
    }

Write the null-terminated string `string`.

    <<writer definitions>>=
    void MgWriteCString(
        MgWriter* writer,
        char const* text)
    {
        MgWriteRange(writer, text, text + strlen(text));
    }

Memory Writer
-------------

    <<writer definitions>>=
    void MemoryWriter_PutChar(
        MgWriter* writer,
        int     value )
    {
        char* cursor = (char*) writer->userData;
        *cursor++ = (char) value;
        writer->userData = cursor;
    }

Initialize a writer that will output data to the specified `buffer`.
It is the responsibility of the user to allocate the right amount of
data (that is, you should probably have used a "counting" writer first).

    <<writer definitions>>=
    void MgInitializeMemoryWriter(
        MgWriter* writer,
        void*   data )
    {
        MgInitializeWriter(
            writer,
            &MemoryWriter_PutChar,
            data );
    }

Counting Writer
---------------

    <<writer definitions>>=
    void CountingWriter_PutChar(
        MgWriter* writer,
        int     value )
    {
        int* counter = (int*) writer->userData;
        ++(*counter);
    }

Initialize a writer that will simply count the number of character written.
Once you've finished writing your data, the provided `counter` will contain
the count.

    <<writer definitions>>=
    void MgInitializeCountingWriter(
        MgWriter* writer,
        int*    counter )
    {
        MgInitializeWriter(
            writer,
            &CountingWriter_PutChar,
            counter );
        *counter = 0;
    }
