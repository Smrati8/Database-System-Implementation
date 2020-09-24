#ifndef GENERICDBFILE_H
#define GENERICDBFILE_H

#include "File.h"

#include <cstring>

class GenericDBFile {
private:
    void DoFileOpenCheck();

protected:
    char dbFileName[100];
    File dbFile;
    bool isDBFileOpen;

    bool isInWriteMode;

    Page readBufferPage;
    off_t currentlyBeingReadPageNumber;

    ComparisonEngine comparisonEngine;

    off_t GetLengthInPages();

    bool GetPageFromDataFile(Page &page, off_t pageNumber);

    void AddPageToDataFile(Page &page, off_t pageNumber);

    int GetRecordFromReadBufferPage(Record &rec);

    // Child classes should add implementation of following methods.
    virtual void SwitchToWriteMode() = 0;

    virtual void SwitchToReadMode() = 0;

    virtual void AddToDBFile(Record &addme) = 0;

    virtual int GetNextFromDBFile(Record &fetchme) = 0;

    virtual int GetNextFromDBFile(Record &fetchme, CNF &cnf, Record &literal) = 0;

public:
    GenericDBFile();

    ~GenericDBFile();

    int Create(const char *fpath);

    int Open(const char *fpath);

    void Add(Record &addme);

    void Load(Schema &myschema, const char *loadpath);

    void MoveFirst();

    int GetNext(Record &fetchme);

    int GetNext(Record &fetchme, CNF &cnf, Record &literal);

    int Close();
};

#endif