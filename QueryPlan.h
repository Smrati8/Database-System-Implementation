#ifndef QUERY_PLAN_H
#define QUERY_PLAN_H

#include <vector>
#include <float.h>

#include "ParseTree.h"
#include "Statistics.h"
#include "Schema.h"
#include "Function.h"
#include "RelOpPlanNode.h"
#include "Comparison.h"
#include "RelOp.h"
#include "PathConfig.h"
#include "DBFile.h"

void HeapPermutation(int *a, int size, int n, vector<int *> *permutations);

struct Query {
    FuncOperator *finalFunction;
    TableList *tables;
    AndList *andList;
    NameList *groupingAtts;
    NameList *attsToSelect;
    int distinctAtts;
    int distinctFunc;
};


class QueryPlan {
private:
    unordered_map<char *, DBFile *> *dbFileMap;
    Query *query;
    Statistics *statistics;

    unordered_map<string, RelOpPlanNode *> groupNameToRelOpNode;
    unordered_map<string, string> relNameToGroupNameMap;

    int nextAvailablePipeId = 1;

    void LoadAllTables();

    void SplitAndList(unordered_map<string, AndList *> *tableSelectionAndList, vector<AndList *> *joins);

    void ApplySelection(unordered_map<string, AndList *> *tableSelectionAndList);

    void RearrangeJoins(vector<AndList *> *joins, vector<AndList *> *joins_arranged);

    void ApplyJoins(vector<AndList *> *joins);

    void ApplyGroupBy();

    void ApplySum();

    void ApplyDuplicateRemoval();

    void ApplyProject();

    static void PrintQueryPlanPostOrder(RelOpPlanNode *node);

    string GetResultantGroupName();

public:
    QueryPlan(unordered_map<char *, DBFile *> *dbFileMap, Statistics *statistics, Query *query);

    ~QueryPlan();

    RelOpPlanNode *GetQueryPlan();

    void Print();

private:
    void MakeQueryPlan();
};

#endif