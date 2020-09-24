#include "HeapDBFile.h"

HeapDBFile::HeapDBFile() {
    currentlyBeingWritenPageNumber = 0;
    writeBufferPage.EmptyItOut();
}

HeapDBFile::~HeapDBFile() {
}

/* ****************************************** ALL OVERRIDDEN METHODS *********************************************** */

void HeapDBFile::SwitchToWriteMode() {
    if (isInWriteMode) return;

    writeBufferPage.EmptyItOut();
    currentlyBeingWritenPageNumber = GetLengthInPages() <= 0 ? 0 : GetLengthInPages() - 1;
    GetPageFromDataFile(writeBufferPage, currentlyBeingWritenPageNumber);
    isInWriteMode = true;
}

void HeapDBFile::SwitchToReadMode() {
    if (!isInWriteMode) return;
    AddPageToDataFile(writeBufferPage, currentlyBeingWritenPageNumber);
    isInWriteMode = false;
}

void HeapDBFile::AddToDBFile(Record &addme) {
    if (writeBufferPage.Append(&addme)) return;

    AddPageToDataFile(writeBufferPage, currentlyBeingWritenPageNumber++);
    writeBufferPage.Append(&addme);
}

int HeapDBFile::GetNextFromDBFile(Record &fetchme) {
    return GetRecordFromReadBufferPage(fetchme);
}

int HeapDBFile::GetNextFromDBFile(Record &fetchme, CNF &cnf, Record &literal) {
    while (GetRecordFromReadBufferPage(fetchme))
        if (comparisonEngine.Compare(&fetchme, &literal, &cnf)) return 1;

    return 0;
}
