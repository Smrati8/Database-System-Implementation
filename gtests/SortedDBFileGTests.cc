#include "gtest/gtest.h"
#include "../DBFile.h"

#include <iostream>
#include <cstring>

char *catalog_path = "catalog";
char *dbfile_dir = "db-files/";
char *tpch_dir ="../tpch-dbgen/";

void GetMataDataFilePath(const char *fpath, char *metadataPath) {
    strcpy(metadataPath, fpath);
    strcat(metadataPath, ".metadata");
}

TEST(SortedDBFileTest, TestMetaDataFile) {
    // Create nation new File.
    char filePath[100];
    sprintf (filePath, "%s%s.bin", dbfile_dir, "nation");

    SortInfo* sortInfo = new SortInfo();
    sortInfo->runLength = 8;
    OrderMaker order;
    order.FromString("1 2:0 ");
    sortInfo->myOrder = &order;

    DBFile dbFile;

    ASSERT_TRUE(dbFile.Create(filePath, sorted, sortInfo));
    dbFile.Close();

    // Check dbFile is created.
    ifstream fIn;
    fIn.open(filePath);
    ASSERT_TRUE(fIn.is_open());
    fIn.close();

    // Check Metadata file is created.
    char metadataPath[100];
    GetMataDataFilePath(filePath, metadataPath);
    fIn.open(metadataPath);
    ASSERT_TRUE(fIn.is_open());

    // Check metadata file i.e
    string readLine;
    // 1) Check dbFile type in metadata file.
    getline(fIn, readLine);
    EXPECT_EQ(stoi(readLine), sorted);

    // 2) Check run length in the dbFile.
    getline(fIn, readLine);
    EXPECT_EQ(stoi(readLine), sortInfo->runLength);

    // 3) Check sort order stored in the metadatfile.
    getline(fIn, readLine);
    EXPECT_EQ(readLine, sortInfo->myOrder->ToString());

    fIn.close();

    // Remove both files.
    remove(filePath);
    remove(metadataPath);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}