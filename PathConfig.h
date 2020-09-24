#ifndef PATH_CONFIG_H
#define PATH_CONFIG_H

#include <stdio.h>
#include <iostream>

using namespace std;
// test settings file should have the
// catalog_dir, dbfile_dir, statistics_path information in separate lines
static const char *settings = "tests/test.cat";

class PathConfig {
    friend class Database;

private:
    static PathConfig *instance;

    char *catalogDir;

    char *dbFileDir;

    char *statisticsFilePath;

    PathConfig();

public:
    static PathConfig *GetInstance() {
        if (instance == NULL) {
            instance = new PathConfig();
        }
        return instance;
    }

    char *GetSchemaPath(const char *tableName);

    char *GetDBFilePath(const char *tableName);

    char *GetMetadataFilePath(const char *filePath);

    char *GetStatisticsFilePath();
};

#endif