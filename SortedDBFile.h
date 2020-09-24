#ifndef SORTED_H
#define SORTED_H

#include "GenericDBFile.h"
#include "File.h"
#include "BigQ.h"
#include "HeapDBFile.h"
#include "Pipe.h"

#include <cstring>

struct SortInfo {
    OrderMaker *myOrder;
    int runLength;
};

class SortedDBFile : public GenericDBFile {

private:
    SortInfo* sortInfo;

    Pipe* inputPipe;
    Pipe* outputPipe;

    OrderMaker* queryOrderMaker;
    bool useSameQueryOrderMaker;


    void MergeCurrentFileAndBigQOutput();

    int GetNextForSortedFile(Record &fetchme, CNF &cnf, Record &literal);
        int CheckForQuery(Record &fetchme, Record &literal);
        bool CheckForCNF(Record &fetchme, CNF &cnf, Record &literal);
        off_t BinarySearch(off_t low, off_t high, Record &literal);
public:
    // constructor and destructor
    explicit SortedDBFile(SortInfo* sortInfo);
    ~SortedDBFile ();

    void SwitchToWriteMode();

    void SwitchToReadMode();

    void AddToDBFile(Record &addme);

    int GetNextFromDBFile(Record &fetchme);

    int GetNextFromDBFile(Record &fetchme, CNF &cnf, Record &literal);
};
#endif