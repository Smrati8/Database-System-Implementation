#include "gtest/gtest.h"
#include "../BigQ.h"
#include "../DBFile.h"
#include "../Schema.h"

#include <fstream>
#include <iostream>

char *catalog_path = "catalog";
char *tpch_dir ="../tpch-dbgen/"; // dir where dbgen tpch files (extension *.tbl) can be found
char *nation_bin_file_name = "nation.bin";

TEST(BigQTest, TestInitializeWorkerThreadDataMethod) {
    WorkerThreadData workerThreadData = {};
    InitializeWorkerThreadData(&workerThreadData);

    ifstream iFile;
    iFile.open(workerThreadData.bigQFileName);
    EXPECT_EQ(!!iFile, true);

    EXPECT_EQ(workerThreadData.currentRunPageNumber, 0);
    EXPECT_EQ(workerThreadData.numberOfRuns, 0);

    EXPECT_FALSE(workerThreadData.currentRunPages == NULL);
}

TEST(BigQTest, TestCleanUpMethod) {
    WorkerThreadData workerThreadData = {};
    InitializeWorkerThreadData(&workerThreadData);
    workerThreadData.outputPipe = new Pipe(10);
    CleanUp(&workerThreadData);

    EXPECT_FALSE(workerThreadData.outputPipe->IsOpen());

    ifstream iFile;
    iFile.open(workerThreadData.bigQFileName);
    EXPECT_EQ(!iFile, true);
}

TEST(BigQTest, TestAddRecordToCurrentRunMethod) {
    WorkerThreadData workerThreadData = {
            .runLength = 1,
            .currentRunPageNumber = 0,
    };
    InitializeWorkerThreadData(&workerThreadData);

    DBFile dbfile;
    dbfile.Create(nation_bin_file_name, heap, NULL);
    Schema *rschema = new Schema (catalog_path, "nation");
    char tbl_path[100]; // construct path of the tpch flat text file
    sprintf (tbl_path, "%s%s.tbl", tpch_dir, "nation");
    dbfile.Load(*rschema, tbl_path);

    int numberOfRecordAdded = 0;
    Record temp;
    while(dbfile.GetNext(temp)) {
        numberOfRecordAdded++;
        AddRecordToCurrentRun(&workerThreadData, &temp);
    }

    int recordsInCurrentRun = 0;
    for (int i=0; i < workerThreadData.runLength; i++) {
        while(workerThreadData.currentRunPages[i].GetFirst(&temp)) {
            recordsInCurrentRun++;
        }
    }
    EXPECT_EQ(numberOfRecordAdded, recordsInCurrentRun);
    CleanUp(&workerThreadData);
    remove(nation_bin_file_name);
}

TEST(BigQTest, TestLoadCurrentRunPriorityQueueMethod) {
    WorkerThreadData workerThreadData = {
            .runLength = 1,
            .currentRunPageNumber = 0,
    };
    InitializeWorkerThreadData(&workerThreadData);

    DBFile dbfile;
    dbfile.Create(nation_bin_file_name, heap, NULL);
    Schema *rschema = new Schema (catalog_path, "nation");
    char tbl_path[100]; // construct path of the tpch flat text file
    sprintf (tbl_path, "%s%s.tbl", tpch_dir, "nation");
    dbfile.Load(*rschema, tbl_path);

    int numberOfRecordAdded = 0;
    Record temp;
    while(dbfile.GetNext(temp)) {
        numberOfRecordAdded++;
        AddRecordToCurrentRun(&workerThreadData, &temp);
    }

    priority_queue<Record*, vector<Record*>, RecordComparator> pq(new OrderMaker());
    LoadCurrentRunPriorityQueue(&workerThreadData, pq);

    EXPECT_EQ(numberOfRecordAdded, pq.size());

    int recordsInCurrentRun = 0;
    for (int i=0; i < workerThreadData.runLength; i++) {
        while(workerThreadData.currentRunPages[i].GetFirst(&temp)) {
            recordsInCurrentRun++;
        }
    }

    EXPECT_EQ(recordsInCurrentRun, 0);
    CleanUp(&workerThreadData);
    remove(nation_bin_file_name);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}