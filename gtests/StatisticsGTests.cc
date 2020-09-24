#include "gtest/gtest.h"
#include "../Statistics.h"


TEST(StatisticsGTests, TestAddRel) {
    // Initialization.
    Statistics statistics;
    char *relName = "relation";
    int numTuples = 1000;

    // Call method AddRel.
    statistics.AddRel(relName, numTuples);

    // Test.
    unordered_map<string, Relation> *groupNameToRelationMap = statistics.GetGroupNameToRelationMap();

    EXPECT_EQ(1, groupNameToRelationMap->size());
    EXPECT_FALSE(groupNameToRelationMap->find(string(relName)) == groupNameToRelationMap->end());
    EXPECT_EQ(numTuples, (*groupNameToRelationMap)[string(relName)].GetNumOfTuples());

    // Update number of tuples
    numTuples = 100;

    // Call method AddRel.
    statistics.AddRel(relName, numTuples);

    // Test.
    EXPECT_EQ(1, groupNameToRelationMap->size());
    EXPECT_FALSE(groupNameToRelationMap->find(string(relName)) == groupNameToRelationMap->end());
    EXPECT_EQ(numTuples, (*groupNameToRelationMap)[string(relName)].GetNumOfTuples());
}

TEST(StatisticsGTests, TestAddAtt) {
    // Test setup.
    Statistics statistics;
    char *relName = "relation";
    char *attName = "attribute";
    string relNameWithAttName = string(relName) + "." + string(attName);
    int numTuples = 1000;
    int numDistincts = 25;
    statistics.AddRel(relName, numTuples);

    // Call method AddRel.
    statistics.AddAtt(relName, attName, numDistincts);

    // Test.
    unordered_map<string, Att> *attNameToAttributeMap = statistics.GetAttNameToAttributeMap();
    EXPECT_EQ(1, attNameToAttributeMap->size());
    EXPECT_FALSE(attNameToAttributeMap->find(relNameWithAttName) == attNameToAttributeMap->end());
    EXPECT_EQ(numDistincts, (*attNameToAttributeMap)[relNameWithAttName].GetNumOfDistinct());

    // Update numDistincts
    numDistincts = 100;

    // Call method AddRel.
    statistics.AddAtt(relName, attName, numDistincts);
    EXPECT_EQ(1, attNameToAttributeMap->size());

    // Test.
    EXPECT_FALSE(attNameToAttributeMap->find(relNameWithAttName) == attNameToAttributeMap->end());
    EXPECT_EQ(numDistincts, (*attNameToAttributeMap)[relNameWithAttName].GetNumOfDistinct());
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}