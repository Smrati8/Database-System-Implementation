#include "gtest/gtest.h"
#include "../Database.h"
#include "../PathConfig.h"

extern "C" {
    int yyparse(void);   // defined in y.tab.c
    struct YY_BUFFER_STATE *yy_scan_string(const char *);
}

TEST(Database, SETQuery) {
    Database database;
    ASSERT_EQ(database.GetOutputType(), STD_OUT);

    char *input = "SET OUTPUT NONE";
    yy_scan_string(input);

    database.ExecuteQuery();
    ASSERT_EQ(database.GetOutputType(), NO_OUT);
}

TEST(Database, CreateQuery) {
    Database database;
    PathConfig *pathConfig = PathConfig::GetInstance();

    char *input = "CREATE TABLE mytable (att1 INTEGER, att2 DOUBLE, att3 STRING) AS HEAP";
    yy_scan_string(input);
    database.ExecuteQuery();

    char *schemaPath = pathConfig->GetSchemaPath("mytable");
    char *filePath = pathConfig->GetDBFilePath("mytable");
    char *metadataPath = pathConfig->GetMetadataFilePath(filePath);

    // Check dbFile is created.
    ifstream fIn;
    fIn.open(filePath);
    ASSERT_TRUE(fIn.is_open());
    fIn.close();

    // Check metadata file is created.
    fIn.open(metadataPath);
    ASSERT_TRUE(fIn.is_open());
    fIn.close();

    // check schema file is created.
    fIn.open(schemaPath);
    ASSERT_TRUE(fIn.is_open());
    fIn.close();

    input = "DROP TABLE mytable";
    yy_scan_string(input);
    database.ExecuteQuery();

    // Check dbFile is removed.
    fIn.open(filePath);
    ASSERT_FALSE(fIn.is_open());
    fIn.close();

    // Check metadata file is removed.
    fIn.open(metadataPath);
    ASSERT_FALSE(fIn.is_open());
    fIn.close();

    // check schema file is removed.
    fIn.open(schemaPath);
    ASSERT_FALSE(fIn.is_open());
    fIn.close();
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}