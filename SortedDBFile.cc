#include "SortedDBFile.h"
#include "GenericDBFile.h"


SortedDBFile::SortedDBFile(SortInfo *sortInfoParameter) {
    sortInfo = sortInfoParameter;

    inputPipe = new Pipe(PIPE_BUFFER_SIZE);
    outputPipe = new Pipe(PIPE_BUFFER_SIZE);

    queryOrderMaker = new OrderMaker();
    useSameQueryOrderMaker = false;
}

SortedDBFile::~SortedDBFile() {
    delete inputPipe;
    delete outputPipe;

    delete queryOrderMaker;
}

/* ****************************************** ALL OVERRIDDEN METHODS *********************************************** */

void SortedDBFile::SwitchToWriteMode() {
    if (isInWriteMode) return;

    BigQ(*inputPipe, *outputPipe, *sortInfo->myOrder, sortInfo->runLength);
    useSameQueryOrderMaker = false;
    isInWriteMode = true;
}

void SortedDBFile::SwitchToReadMode() {
    if (!isInWriteMode) return;

    isInWriteMode = false;
    MergeCurrentFileAndBigQOutput();
}

void SortedDBFile::AddToDBFile(Record &addme) {
    inputPipe->Insert(&addme);
}

int SortedDBFile::GetNextFromDBFile(Record &fetchme) {
    return GetRecordFromReadBufferPage(fetchme);
}

int SortedDBFile::GetNextFromDBFile(Record &fetchme, CNF &cnf, Record &literal) {
    if (!useSameQueryOrderMaker) {
        cnf.GetCommonSortOrder(*sortInfo->myOrder, *queryOrderMaker);
    }
    return GetNextForSortedFile(fetchme, cnf, literal);
}

/* ****************************************** ALL PRIVATE METHODS *********************************************** */

void SortedDBFile::MergeCurrentFileAndBigQOutput() {

    // Shut down input pipe of BigQ so that BigQ gives sorted records in output pipe.
    inputPipe->ShutDown();

    // Create Tempt file as Heap and keep on adding records in sorted order.
    char tempFileName[100];
    strcpy(tempFileName, dbFileName);
    strcat(tempFileName, ".temp");
    HeapDBFile tempFile;
    tempFile.Create(tempFileName);

    // Move first current file for reading.
    MoveFirst();
    Record currentFileRecord;
    bool currentFileNotFullyConsumed = GetRecordFromReadBufferPage(currentFileRecord);

    // Output pipe variables.
    Record outputPipeRecord;
    bool outputPipeNotFullyConsumed = outputPipe->Remove(&outputPipeRecord);

    // If either of the stream is fully consumed - Break;
    while (outputPipeNotFullyConsumed && currentFileNotFullyConsumed) {
        // If currentFileRecord is less
        if (comparisonEngine.Compare(&currentFileRecord, &outputPipeRecord, sortInfo->myOrder) < 1) {
            tempFile.Add(currentFileRecord);
            currentFileNotFullyConsumed = GetRecordFromReadBufferPage(currentFileRecord);
        }
            // If BigQ output pipe's record is less.
        else {
            tempFile.Add(outputPipeRecord);
            outputPipeNotFullyConsumed = outputPipe->Remove(&outputPipeRecord);
        }
    }

    // If output pipe is not fully consumed.
    while (outputPipeNotFullyConsumed) {
        tempFile.Add(outputPipeRecord);
        outputPipeNotFullyConsumed = outputPipe->Remove(&outputPipeRecord);
    }

    // If current file records are not fully consumed.
    while (currentFileNotFullyConsumed) {
        tempFile.Add(currentFileRecord);
        currentFileNotFullyConsumed = GetRecordFromReadBufferPage(currentFileRecord);
    }

    // Close both files
    tempFile.Close();
    dbFile.Close();

    // remove the old file.
    remove(dbFileName);

    // rename the file name.
    rename(tempFileName, dbFileName);
    dbFile.Open(1, dbFileName);
}

int SortedDBFile::GetNextForSortedFile(Record &fetchme, CNF &cnf, Record &literal) {
    if (queryOrderMaker->isEmpty()) {
        while (GetRecordFromReadBufferPage(fetchme)) {
            if (CheckForCNF(fetchme, cnf, literal)) return 1;
        }
        return 0;
    }

    if (!useSameQueryOrderMaker) {
        useSameQueryOrderMaker = true;
        bool doBinarySearch = true;
        while (readBufferPage.GetFirst(&fetchme)) {
            if (CheckForCNF(fetchme, cnf, literal)) return 1;
            int comparisonValue = CheckForQuery(fetchme, literal);
            if (comparisonValue == 1) return 0;
            if (comparisonValue == 0) {
                if (CheckForCNF(fetchme, cnf, literal)) return 1;
                doBinarySearch = false;
                break;
            }
        }
        if (doBinarySearch) {
            if (GetLengthInPages() - 1 <= currentlyBeingReadPageNumber + 1) return 0;
            currentlyBeingReadPageNumber = BinarySearch(++currentlyBeingReadPageNumber, GetLengthInPages() - 1,
                                                        literal);
        }
    }

    while (GetRecordFromReadBufferPage(fetchme)) {
        if (CheckForCNF(fetchme, cnf, literal)) return 1;
        if (CheckForQuery(fetchme, literal)) return 0;
    }
    return 0;
}

int SortedDBFile::CheckForQuery(Record &fetchme, Record &literal) {
    return comparisonEngine.Compare(&literal, queryOrderMaker, &fetchme, sortInfo->myOrder);
}

bool SortedDBFile::CheckForCNF(Record &fetchme, CNF &cnf, Record &literal) {
    return comparisonEngine.Compare(&fetchme, &literal, &cnf);
}

off_t SortedDBFile::BinarySearch(off_t low, off_t high, Record &literal) {
    Page bufferPage;
    Record temp;

    // Base condition or Break condition
    if (low == high) return low;

    // Get first record of the middle page.
    off_t mid = (low + high) / 2;
    dbFile.GetPage(&bufferPage, mid);
    bufferPage.GetFirst(&temp);

    if (comparisonEngine.Compare(&literal, queryOrderMaker, &temp, sortInfo->myOrder) > 0) {
        return BinarySearch(mid + 1, high, literal);
    }
    return BinarySearch(low, mid - 1, literal);
}