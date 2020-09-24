#ifndef REL_OP_H
#define REL_OP_H

#include "Pipe.h"
#include "DBFile.h"
#include "Record.h"
#include "Function.h"

static char *SUM_ATT_NAME = "SUM";
static Attribute doubleAtt = {SUM_ATT_NAME, Double};
static Schema sumSchema("sum_schema", 1, &doubleAtt);

class RelationalOp {

protected:
    pthread_t thread;
    int runLength;

public:
    RelationalOp();

    // blocks the caller until the particular relational operator
    // has run to completion
    void WaitUntilDone();

    // tell us how much internal memory the operation can use
    void Use_n_Pages(int n);
};

struct SelectFileData {
    DBFile *dbFile;
    Pipe *outputPipe;
    CNF *cnf;
    Record *literal;
};

void *SelectFileThreadMethod(void *threadData);

class SelectFile : public RelationalOp {
public:
    void Run(DBFile &inFile, Pipe &outPipe, CNF &selOp, Record &literal);
};

struct SelectPipeData {
    Pipe *inputPipe;
    Pipe *outputPipe;
    CNF *cnf;
    Record *literal;
};

void *SelectPipeThreadMethod(void *threadData);

class SelectPipe : public RelationalOp {
public:
    void Run(Pipe &inPipe, Pipe &outPipe, CNF &selOp, Record &literal);
};

struct ProjectData {
    Pipe *inputPipe;
    Pipe *outputPipe;
    int *keepMe;
    int numAttsInput;
    int numAttsOutput;
};

void *ProjectThreadMethod(void *threadData);

class Project : public RelationalOp {
public:
    void Run(Pipe &inPipe, Pipe &outPipe, int *keepMe, int numAttsInput, int numAttsOutput);
};

struct JoinData {
    Pipe *leftInputPipe;
    Pipe *rightInputPipe;
    Pipe *outputPipe;
    CNF *cnf;
    Record *literal;
    int runLength;
};

void *JoinThreadMethod(void *threadData);

void NestedBlockJoin(Pipe *leftInputPipe, Pipe *rightInputPipe, Pipe *outputPipe, int runLength);

void LoadVectorFromBlock(vector<Record *> *loadMe, Page *block, int blockLength);

void JoinUsingSortMerge(Pipe *leftInputPipe, Pipe *rightInputPipe, Pipe *outputPipe,
                        OrderMaker *leftOrderMaker, OrderMaker *rightOrderMaker);

void JoinTableBlocks(vector<Record *> *leftBlockRecords, vector<Record *> *rightBlockRecords, Pipe *outputPipe);

class Join : public RelationalOp {
public:
    void Run(Pipe &inPipeL, Pipe &inPipeR, Pipe &outPipe, CNF &selOp, Record &literal);
};

struct DuplicateRemovalData {
    Pipe *inputPipe;
    Pipe *outputPipe;
    Schema *schema;
    int runLength;
};

void *DuplicateRemovalThreadMethod(void *threadData);

class DuplicateRemoval : public RelationalOp {
public:
    void Run(Pipe &inPipe, Pipe &outPipe, Schema &mySchema);
};

struct SumData {
    Pipe *inputPipe;
    Pipe *outputPipe;
    Function *computeMe;
    int distinctFunc;
};

void *SumThreadMethod(void *threadData);

void SumAll(Pipe *inPipe, Pipe *outPipe, Function *computeMe);

void SumDistinct(Pipe *inPipe, Pipe *outPipe, Function *computeMe);

class Sum : public RelationalOp {
public:
    void Run(Pipe &inPipe, Pipe &outPipe, Function &computeMe, int distinctFunc);
};

struct GroupByData {
    Pipe *inputPipe;
    Pipe *outputPipe;
    OrderMaker *groupAtts;
    Function *computeMe;
    int distinctFunc;

    int runLength;
};

void *GroupByThreadMethod(void *threadData);

void AddGroupByRecordToPipe(Pipe *outputPipe, Record *tableRecord, Record *sumRecord, OrderMaker *order);

class GroupBy : public RelationalOp {
public:
    void Run(Pipe &inPipe, Pipe &outPipe, OrderMaker &groupAtts, Function &computeMe, int distinctFunc);
};

struct WriteOutData {
    Pipe *inputPipe;
    FILE *outputFile;
    Schema *schema;
};

void *WriteOutThreadMethod(void *threadData);

class WriteOut : public RelationalOp {
public:
    void Run(Pipe &inPipe, FILE *outFile, Schema &mySchema);
};

#endif