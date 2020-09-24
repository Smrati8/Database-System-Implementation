#include "GenericDBFile.h"

GenericDBFile::GenericDBFile() {
    isDBFileOpen = false;

    isInWriteMode = false;
    currentlyBeingReadPageNumber = 0;
}

GenericDBFile::~GenericDBFile() {
}

/* **************************************** ALL PUBLIC METHODS *************************************************** */

int GenericDBFile::Create(const char *fpath) {
    strcpy(dbFileName, fpath);
    return isDBFileOpen = dbFile.Open(0, const_cast<char *>(fpath));
}

int GenericDBFile::Open(const char *fpath) {
    strcpy(dbFileName, fpath);
    isDBFileOpen = dbFile.Open(1, const_cast<char *>(fpath));

    // Return 0 if it is not able to open the file.
    if (!isDBFileOpen) return isDBFileOpen;

    // Move read pointer to the first page.
    MoveFirst();

    return isDBFileOpen;
}

void GenericDBFile::Add(Record &addme) {

    // Throw error if file is not open.
    DoFileOpenCheck();

    SwitchToWriteMode();

    AddToDBFile(addme);
}

void GenericDBFile::Load(Schema &myschema, const char *loadpath) {

    // Throw error if file is not open.
    DoFileOpenCheck();

    SwitchToWriteMode();

    // Load record from table file and add record to the file recursively,
    // while all the records are loaded from table file.
    FILE *tableFile = fopen(loadpath, "r");
    Record temp;
    while (temp.SuckNextRecord(&myschema, tableFile)) {
        AddToDBFile(temp);
    }
}

void GenericDBFile::MoveFirst() {

    // Throw error if file is not open.
    DoFileOpenCheck();

    SwitchToReadMode();

    // Load read page buffer with the first i.e 0th page.
    currentlyBeingReadPageNumber = 0;
    GetPageFromDataFile(readBufferPage, currentlyBeingReadPageNumber);
}

int GenericDBFile::GetNext(Record &fetchme) {

    // Return 0 if file is not open.
    if (!isDBFileOpen) return 0;

    SwitchToReadMode();

    return GetNextFromDBFile(fetchme);
}

int GenericDBFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) {

    // Return 0 if file is not open.
    if (!isDBFileOpen) return 0;

    SwitchToReadMode();

    return GetNextFromDBFile(fetchme, cnf, literal);
}

int GenericDBFile::Close() {

    // Return 0 if file is not open.
    if (!isDBFileOpen) return 0;

    SwitchToReadMode();

    // Close the file.
    dbFile.Close();
    return !(isDBFileOpen = false);
}

/* **************************************** ALL PROTECTED METHODS ************************************************** */

off_t GenericDBFile::GetLengthInPages() {
    return dbFile.GetLength() - 1;
}

bool GenericDBFile::GetPageFromDataFile(Page &page, off_t pageNumber) {
    page.EmptyItOut();

    // if file contains that many pages, load the page from the file.
    if (pageNumber < 0 || GetLengthInPages() - 1 < pageNumber) return false;

    dbFile.GetPage(&page, pageNumber);
    return true;
}

void GenericDBFile::AddPageToDataFile(Page &page, off_t pageNumber) {
    dbFile.AddPage(&page, pageNumber);
    page.EmptyItOut();
}

int GenericDBFile::GetRecordFromReadBufferPage(Record &rec) {
    if (readBufferPage.GetFirst(&rec)) return 1;

    if (!GetPageFromDataFile(readBufferPage, ++currentlyBeingReadPageNumber)) return 0;
    return GetRecordFromReadBufferPage(rec);
}

/* **************************************** ALL PRIVATE METHODS ************************************************** */
void GenericDBFile::DoFileOpenCheck() {
    if (!isDBFileOpen) {
        std::cerr << "BAD: File is not open!\n";
        exit(1);
    }
}