#include "gtest/gtest.h"
#include "../DBFile.h"
#include "../RelOp.h"
#include "../File.h"
#include "../Defs.h"
#include "../Pipe.h"
#include "../Comparison.h"
#include "pthread.h"

char *catalog_path = "catalog";
char *dbfile_dir = "db-files/";
char *tpch_dir ="../tpch-dbgen/";

char *nation = "nation";
// DB File directory
char dbFileDir[100];

Schema nationSchema(catalog_path, nation);

// Create nation schema.
DBFile nationFile;

class RelOpGTests : public ::testing::Test {
public:
    void SetUp() {
        sprintf (dbFileDir, "%s%s.bin", dbfile_dir, nation);

        // Create DBFile.
        nationFile.Create(dbFileDir, heap, NULL);

        char tbl_path[100]; // construct path of the tpch flat text file
        sprintf (tbl_path, "%s%s.tbl", tpch_dir, nation);

        // Load the heap file.
        nationFile.Load(nationSchema, tbl_path);
    }
};

TEST_F(RelOpGTests, TestLoadVectorFromBlock) {

    vector<Record *> records;
    Page *recordsBlock = new Page[1];

    int numOfRecordsInBlock = 0;
    nationFile.MoveFirst();
    Record temp;

    while (nationFile.GetNext(temp)) {
        numOfRecordsInBlock++;
        recordsBlock[0].Append(&temp);
    }

    LoadVectorFromBlock(&records, recordsBlock, 0);
    EXPECT_EQ(records.size(), numOfRecordsInBlock);
}

TEST_F(RelOpGTests, TestNestedBlockJoin) {

    Pipe leftInputPipe(PIPE_BUFFER_SIZE), rightInputPipe(PIPE_BUFFER_SIZE), outputPipe(625);

    int numOfRecordsInBlock = 0;
    nationFile.MoveFirst();
    Record temp;
    nationFile.MoveFirst();
    while (nationFile.GetNext(temp)) {
        numOfRecordsInBlock++;
        leftInputPipe.Insert(&temp);
    }
    leftInputPipe.ShutDown();

    nationFile.MoveFirst();
    while (nationFile.GetNext(temp)) {
        rightInputPipe.Insert(&temp);
    }
    rightInputPipe.ShutDown();

    NestedBlockJoin(&leftInputPipe, &rightInputPipe, &outputPipe, 1);
    outputPipe.ShutDown();

    int numberOfRecordsAfterNestedBlockJoin = 0;
    while(outputPipe.Remove(&temp)) {
        numberOfRecordsAfterNestedBlockJoin++;
    }

    EXPECT_EQ(numberOfRecordsAfterNestedBlockJoin, 625);
}


TEST_F(RelOpGTests, TestJoinTableBlocks) {

    Pipe outputPipe(625);

    vector<Record *> leftBlockRecords;
    Page *recordsBlock = new Page[1];
    Record temp;

    int numOfRecordsInLeftBlock = 0;
    nationFile.MoveFirst();
    while (nationFile.GetNext(temp)) {
        numOfRecordsInLeftBlock++;
        recordsBlock[0].Append(&temp);
    }
    LoadVectorFromBlock(&leftBlockRecords, recordsBlock, 0);

    vector<Record *> rightBlockRecords;

    int numOfRecordsInRightBlock = 0;
    nationFile.MoveFirst();
    while (nationFile.GetNext(temp)) {
        numOfRecordsInRightBlock++;
        recordsBlock[0].Append(&temp);
    }
    LoadVectorFromBlock(&rightBlockRecords, recordsBlock, 0);

    JoinTableBlocks(&leftBlockRecords, &rightBlockRecords, &outputPipe);
    int numberOfRecordsAfterNestedBlockJoin = 0;
    outputPipe.ShutDown();
    while(outputPipe.Remove(&temp)) {
        numberOfRecordsAfterNestedBlockJoin++;
    }

    EXPECT_EQ(numberOfRecordsAfterNestedBlockJoin, 625);
}

TEST_F(RelOpGTests, TestAddGroupByRecordToPipe) {

    Record nationRecord;
    nationFile.MoveFirst(); nationFile.GetNext(nationRecord);

    Pipe outputPipe(PIPE_BUFFER_SIZE);
    double sum = 100.0;

    OrderMaker nationOrderMaker(&nationSchema);
    string nationRecordString = nationRecord.ToString(&nationOrderMaker);

    int nationOrderMakerNumAtts = nationOrderMaker.GetNumAtts();
    Attribute *nationSchemaAtts = nationSchema.GetAtts();
    int *nationOrderMakerAtts = nationOrderMaker.GetAtts();

    int sumNumAtts = sumSchema.GetNumAtts();
    Attribute *sumAtts = sumSchema.GetAtts();

    int groupBySchemaNumAtts = nationOrderMakerNumAtts + sumNumAtts;
    Attribute *groupByAtts = new Attribute[groupBySchemaNumAtts];

    for (int i=0; i < sumNumAtts; i++) {
        groupByAtts[i] = sumAtts[i];
    }
    for (int i=0; i < nationOrderMakerNumAtts; i++) {
        groupByAtts[sumNumAtts + i] = nationSchemaAtts[nationOrderMakerAtts[i]];
    }

    Schema groupBySchema("group_by_schema", groupBySchemaNumAtts, groupByAtts);

    // Call AddGroupByRecordToPipe method for testing.
    AddGroupByRecordToPipe(&outputPipe, &nationRecord, sum, &nationOrderMaker);
    Record groupByRecord;
    outputPipe.Remove(&groupByRecord);

    EXPECT_EQ(groupByRecord.ToString(&groupBySchema),
            (std::to_string(sum) + "|").c_str() + nationRecordString);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}