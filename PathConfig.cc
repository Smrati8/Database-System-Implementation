#include "PathConfig.h"

PathConfig *PathConfig::instance = NULL;

PathConfig::PathConfig() {
    FILE *fp = fopen(settings, "r");
    if (fp) {
        char *mem = (char *) malloc(80 * 3);
        catalogDir = &mem[0];
        dbFileDir = &mem[80];
        statisticsFilePath = &mem[160];

        char line[80];
        fgets(line, 80, fp);
        sscanf(line, "%s\n", catalogDir);
        fgets(line, 80, fp);
        sscanf(line, "%s\n", dbFileDir);
        fgets(line, 80, fp);
        sscanf(line, "%s\n", statisticsFilePath);
        if (!(catalogDir && dbFileDir && statisticsFilePath)) {
            cerr << " Test settings file 'test.cat' not in correct format.\n";
            free(mem);
            exit(1);
        }
    } else {
        cerr << " Test settings files 'test.cat' missing \n";
        exit(1);
    }
}

char *PathConfig::GetSchemaPath(const char *tableName) {
    char *path = new char[100];
    sprintf(path, "%s%s.schema", catalogDir, tableName);
    return path;
}

char *PathConfig::GetDBFilePath(const char *tableName) {
    char *path = new char[100];
    sprintf(path, "%s%s.bin", dbFileDir, tableName);
    return path;

}

char *PathConfig::GetMetadataFilePath(const char *filePath) {
    char *path = new char[100];
    // Change this to "%s.bin.metadata"
    sprintf(path, "%s.metadata", filePath);
    return path;
}

char *PathConfig::GetStatisticsFilePath() {
    return this->statisticsFilePath;
}