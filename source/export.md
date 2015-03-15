Export
------

Find an existing attribute with the given `id`.
Return NULL if not found.

    <<global:export definitions>>=
    MgAttribute* MgFindAttribute(
        MgElement*  pp,
        char const* id )
    {
        MgString idString = { id, id + strlen(id) };
        for(MgAttribute* attr = pp->firstAttr; attr; attr = attr->next)
        {
            if( MgStringsAreEqual(attr->id, idString) )
                return attr;
        }
        return 0;
    }

Before writing an output file, we always compare the data we plan to write
against the contents of the file on disk.
If the two match, then we skip writing the file.
This helps avoiding "touching" disk files and inadvertently causing code rebuilds.

    <<export definitions>>=
    MgBool TextIsSameAsFileOnDisk(
        MgString  text,
        char const* filePath)
    {
        MgReader reader;
        FILE* file = fopen(filePath, "rb");
        if( !file )
            return MG_FALSE;

        MgInitializeStringReader(&reader, text);

        for(;;)
        {
            int c = fgetc(file);
            int d = MgGetChar(&reader);
            if( c != d )
            {
                return MG_FALSE;
            }

            if( c == -1 )
                return MG_TRUE;
        }
    }

Write the string `text` to the file specified by `path`, but first check
whether there is already a file on disk with that path with exactly the same
text (in which case don't write anything). This avoids triggerring unneeded
builds for build systems that check file modification times (e.g., `make`).

    <<export definitions>>=
    void MgWriteTextToFile(
        MgString      text,
        char const* filePath)
    {
        if( TextIsSameAsFileOnDisk(text, filePath) )
        {
            return;
        }

        FILE* file = fopen(filePath, "wb");
        if( !file )
        {
            fprintf(stderr, "Failed to open \"%s\" for writing\n", filePath);
            return;
        }

        fwrite(text.begin, 1, text.end - text.begin, file);

        fclose(file);
    }
