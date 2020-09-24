#include "gtest/gtest.h"
#include "../QueryPlan.h"
#include "RelOpPlanNode.h"

#include <vector>

TEST(QueryPlanGTests, HeapPermutation) {
    int a[6] = {1, 2, 3, 4, 5, 6};
    vector<int *> permutations;

    HeapPermutation(a, 6, 6, &permutations);

    ASSERT_EQ(permutations.size(), 720);
}

TEST(QueryPlanGTests, QueryPlanNode) {
    RelOpNode relOp1(NULL, NULL);
    RelOpNode relOp2(NULL, NULL);
    RelOpNode relOp3(&relOp1, &relOp2);

    ASSERT_TRUE(relOp3.child1 == &relOp1);
    ASSERT_TRUE(relOp3.child2 == &relOp2);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}