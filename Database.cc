#include "Database.h"

Database::Database() {
}

Database::~Database() {}

void Database::TurnOn() {
}

void Database::ExecuteQuery() {
    // Parse the query.
    yyparse();

    switch (queryType) {
        case CREATE:
            ExecuteCreateQuery();
            cerr << "\nTable created successfully.\n";
            break;
        case INSERT:
            ExecuteInsertQuery();
            cerr << "\nTable loaded successfully.\n";
            break;
        case DROP:
            ExecuteDropQuery();
            cerr << "\nTable dropped successfully.\n";
            break;
        case SELECT:
            ExecuteSelectQuery();
            cerr << "\nQuery ran successfully.\n";
            break;
        case SET:
            this->queryOutputType = outputType;
            this->outputFileName = fileName;
            cerr << "\nOutput mode changed successfully.\n";
            break;
    }
}

void Database::ShutDown() {
}

OutputType Database::GetOutputType() {
    return this->queryOutputType;
}

void Database::ExecuteCreateQuery() {
    PathConfig *pathConfig = PathConfig::GetInstance();

    // Create new Schema.
    Schema *newSchema = CreateNewSchema(tables->tableName, createAtts);

    // Save schema in the file.
    newSchema->Write(pathConfig->GetSchemaPath(tables->tableName), tables->tableName);

    // Get file path from path config
    char *filePath = pathConfig->GetDBFilePath(tables->tableName);

    DBFile dbFile;
    switch (fileType) {
        case HEAP:
            dbFile.Create(filePath, heap, NULL);
            break;
        case SORTED: {
            SortInfo *sortInfo = new SortInfo();
            sortInfo->runLength = DEFAULT_RUN_LENGTH;
            sortInfo->myOrder = new OrderMaker(newSchema, sortAtts);
            dbFile.Create(filePath, sorted, sortInfo);
            break;
        }
    }
    dbFile.Close();
}

void Database::ExecuteInsertQuery() {
    DBFile dbFile;

    // Get file path from path config
    PathConfig *pathConfig = PathConfig::GetInstance();
    char *filePath = pathConfig->GetDBFilePath(tables->tableName);

    dbFile.Open(filePath);

    Schema schema(pathConfig->GetSchemaPath(tables->tableName), tables->tableName);

    dbFile.Load(schema, fileName);

    dbFile.Close();
}

void Database::ExecuteDropQuery() {

    PathConfig *pathConfig = PathConfig::GetInstance();
    char *filePath = pathConfig->GetDBFilePath(tables->tableName);

    // remove the binary file.
    remove(filePath);

    // remove the metadata file.
    remove(pathConfig->GetMetadataFilePath(filePath));

    // remove the schema.
    remove(pathConfig->GetSchemaPath(tables->tableName));
}

void Database::ExecuteSelectQuery() {
    TableList *tableList = tables;

    // Get file path from path config
    PathConfig *pathConfig = PathConfig::GetInstance();

    // Open All db files.
    unordered_map<char *, DBFile *> dbFileMap;
    while (tableList) {
        DBFile *dbFile = new DBFile();

        dbFile->Open(pathConfig->GetDBFilePath(tableList->tableName));
        dbFile->MoveFirst();

        dbFileMap[tableList->tableName] = dbFile;

        tableList = tableList->next;
    }

    // Read Statistic to generate query plan.
    Statistics statistics;
    statistics.Read(pathConfig->GetStatisticsFilePath());

    // Build Query.
    Query query = {finalFunction, tables, boolean, groupingAtts, attsToSelect, distinctAtts, distinctFunc};

    // Generate query plan.
    QueryPlan queryPlan(&dbFileMap, &statistics, &query);

    if (queryOutputType == NO_OUT) {
        queryPlan.Print();
    } else {
        // Get Query Plan.
        RelOpPlanNode *planTree = queryPlan.GetQueryPlan();

        // Run Query Plan.
        QueryRunner queryRunner(planTree, queryOutputType, outputFileName);
        queryRunner.Run();
    }

    // Close all tables.
    for (auto const &dbFileMapItem : dbFileMap) {
        dbFileMapItem.second->Close();
    }
}

Schema *Database::CreateNewSchema(char *tableName, NameList *attsNameList) {
    NameList *nameList = attsNameList;

    int numAtts = 0;
    while (nameList) {
        numAtts++;
        nameList = nameList->next;
    }

    Attribute *atts = new Attribute[numAtts];
    nameList = attsNameList;

    for (int i = 0; i < numAtts; i++) {
        atts[i].name = nameList->name;
        switch (nameList->type) {
            case INT:
                atts[i].myType = Int;
                break;
            case DOUBLE:
                atts[i].myType = Double;
                break;
            case STRING:
                atts[i].myType = String;
                break;
        }
        nameList = nameList->next;
    }

    Schema *newSchema = new Schema(tableName, numAtts, atts);
    return newSchema;
}

