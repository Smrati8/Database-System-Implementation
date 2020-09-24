#ifndef QUERY_RUNNER_H
#define QUERY_RUNNER_H

#include "RelOpPlanNode.h"
#include "DBFile.h"
#include "RelOp.h"

#include <unordered_map>

class QueryRunner {
private:
    unordered_map<int, Pipe *> pipes;

    RelOpPlanNode *planNode;

    OutputType queryOutputType;

    char *outputFileName;

    void ExecutePlanNode(RelOpPlanNode *relOpPlanNode);

    void ApplyRelOp(RelOpPlanNode *planNode);

    void OutputQueryResult();

    Pipe *GetPipe(int pipeId);

public:
    QueryRunner(RelOpPlanNode *relOpPlanNode, OutputType queryOutputType, char *outputFileName);

    void Run();
};

#endif