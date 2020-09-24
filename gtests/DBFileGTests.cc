#include "gtest/gtest.h"
#include "../DBFile.h"

#include <iostream>

// Varibales for file creation path.
const char *dbfile_dir = ""; // dir where binary heap files should be stored
const char *table_name ="lineitem";
char rpath[100];
DBFile dbFile;

// Test case to test success of DBFile::Create method.
TEST(DBFile, CreateTestSuccess) {
    sprintf (rpath, "%s%s.bin", dbfile_dir, table_name);
    int fileCreatedStatus = dbFile.Create(rpath, heap, NULL);

    EXPECT_EQ(fileCreatedStatus, 1);
}

// Test case to test success of DBFile::Open method.
TEST(DBFile, OpenTestSuccess) {
    sprintf (rpath, "%s%s.bin", dbfile_dir, table_name);
    dbFile.Create(rpath, heap, NULL);

    // First closing the file, so that it opens the file successfully.
    dbFile.Close();

    int readStatus = dbFile.Open(rpath);
    EXPECT_EQ(readStatus, 1);
}

// Test case to test failure of DBFile::Open method.
TEST(DBFile, OpenTestFailure) {

    // "random.bin" file should not be available in the dbfile_dir.
    int readStatus = dbFile.Open("random.bin");
    EXPECT_EQ(readStatus, 0);
}

// Test case to test success and failure of DBFile::Close method.
TEST(DBFile, CloseTestSuccessAndFailure) {
    sprintf (rpath, "%s%s.bin", dbfile_dir, table_name);
    dbFile.Create(rpath, heap, NULL);

    int closeStatus = dbFile.Close();
    EXPECT_EQ(closeStatus, 1);

    closeStatus = dbFile.Close();
    EXPECT_EQ(closeStatus, 0);
}

void TearDown() {
    sprintf (rpath, "%s%s.bin", dbfile_dir, table_name);
    dbFile.Close();
    remove(rpath);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}