#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <unordered_map>

#include "DBFile.h"
#include "PathConfig.h"
#include "Statistics.h"
#include "QueryPlan.h"
#include "QueryRunner.h"

using namespace std;

extern "C" {
int yyparse(void);   // defined in y.tab.c
}

extern QueryType queryType;
extern OutputType outputType;
extern struct NameList *createAtts;
extern struct NameList *sortAtts;
extern FileType fileType;
extern char *fileName;
extern struct TableList *tables;
extern struct FuncOperator *finalFunction;
extern struct AndList *boolean;
extern struct NameList *groupingAtts;
extern struct NameList *attsToSelect;
extern int distinctAtts;
extern int distinctFunc;


class Database {
private:

    OutputType queryOutputType = STD_OUT;

    char *outputFileName;

    void ExecuteCreateQuery();

    Schema *CreateNewSchema(char *tableName, NameList *attsNameList);

    void ExecuteInsertQuery();

    void ExecuteDropQuery();

    void ExecuteSelectQuery();

public:

    Database();

    void TurnOn();

    void ExecuteQuery();

    void ShutDown();

    OutputType GetOutputType();

    ~Database();
};

#endif