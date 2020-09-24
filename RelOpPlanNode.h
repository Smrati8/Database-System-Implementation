#ifndef QUERY_PLAN__NODE_H
#define QUERY_PLAN__NODE_H

#include "Schema.h"
#include "Function.h"
#include "Comparison.h"
#include "DBFile.h"

enum RelOpPlanNodeType {
    SELECT_FILE,
    SELECT_PIPE,
    JOIN,
    GROUP_BY,
    SUM,
    DUPLICATE_REMOVAL,
    PROJECT,
};

class RelOpPlanNode {
public:
    RelOpPlanNodeType type;

    RelOpPlanNode *child1 = nullptr;
    int inputPipeId1 = -1;

    RelOpPlanNode *child2 = nullptr;
    int inputPipeId2 = -1;

    Schema *outputSchema = nullptr;
    int outputPipeId = -1;

    RelOpPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    virtual void Print();
};


class DuplicateRemovalPlanNode : public RelOpPlanNode {
public:
    Schema *inputSchema = nullptr;

    DuplicateRemovalPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    void Print();
};


class GroupByPlanNode : public RelOpPlanNode {
public:
    OrderMaker *groupAtts = nullptr;
    Function *computeMe = nullptr;
    int distinctFunc = 0;

    GroupByPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    void Print();
};


class JoinPlanNode : public RelOpPlanNode {
public:
    CNF *selOp = nullptr;
    Record *literal = nullptr;

    JoinPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    void Print();
};


class SelectFilePlanNode : public RelOpPlanNode {
public:
    DBFile *dbFile = nullptr;
    CNF *selOp = nullptr;
    Record *literal = nullptr;

    SelectFilePlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    void Print();
};


class SelectPipePlanNode : public RelOpPlanNode {
public:
    CNF *selOp = nullptr;
    Record *literal = nullptr;

    SelectPipePlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    void Print();
};

class SumPlanNode : public RelOpPlanNode {
public:
    Function *computeMe = nullptr;
    int distinctFunc = 0;

    SumPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    void Print();
};

class ProjectPlanNode : public RelOpPlanNode {
public:
    int *keepMe = nullptr;
    int numAttsInput;
    int numAttsOutput;

    ProjectPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2);

    void Print();
};

#endif