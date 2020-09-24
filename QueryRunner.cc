#include "QueryRunner.h"

QueryRunner::QueryRunner(RelOpPlanNode *relOpPlanNode, OutputType queryOutputType, char *outputFileName) {
    this->planNode = relOpPlanNode;
    this->queryOutputType = queryOutputType;
    this->outputFileName = outputFileName;
}

void QueryRunner::Run() {
    ExecutePlanNode(planNode);

    OutputQueryResult();
}

void QueryRunner::ExecutePlanNode(RelOpPlanNode *relOpPlanNode) {
    if (!relOpPlanNode) return;

    ExecutePlanNode(relOpPlanNode->child1);
    ExecutePlanNode(relOpPlanNode->child2);

    ApplyRelOp(relOpPlanNode);
}

void QueryRunner::ApplyRelOp(RelOpPlanNode *planNode) {
    switch (planNode->type) {
        case SELECT_FILE: {
            SelectFilePlanNode *sf = dynamic_cast<SelectFilePlanNode *>(planNode);

            SelectFile *selectFile = new SelectFile();
            selectFile->Run(*sf->dbFile, *GetPipe(sf->outputPipeId), *sf->selOp, *sf->literal);
            break;
        }

        case SELECT_PIPE: {
            SelectPipePlanNode *sp = dynamic_cast<SelectPipePlanNode *>(planNode);
            SelectPipe *selectPipe = new SelectPipe();
            selectPipe->Run(*GetPipe(sp->inputPipeId1), *GetPipe(sp->outputPipeId), *sp->selOp, *sp->literal);
            break;
        }

        case JOIN: {
            JoinPlanNode *j = dynamic_cast<JoinPlanNode *>(planNode);
            Join *join = new Join();
            join->Run(*GetPipe(j->inputPipeId1), *GetPipe(j->inputPipeId2), *GetPipe(j->outputPipeId), *j->selOp,
                      *j->literal);
            break;
        }

        case GROUP_BY: {
            GroupByPlanNode *g = dynamic_cast<GroupByPlanNode *>(planNode);
            GroupBy *groupBy = new GroupBy();
            groupBy->Run(*GetPipe(g->inputPipeId1), *GetPipe(g->outputPipeId), *g->groupAtts, *g->computeMe,
                         g->distinctFunc);
            break;
        }

        case SUM: {
            SumPlanNode *s = dynamic_cast<SumPlanNode *>(planNode);
            Sum *sum = new Sum();
            sum->Run(*GetPipe(s->inputPipeId1), *GetPipe(s->outputPipeId), *s->computeMe, s->distinctFunc);
            break;
        }

        case PROJECT: {
            ProjectPlanNode *p = dynamic_cast<ProjectPlanNode *>(planNode);
            Project *project = new Project();
            project->Run(*GetPipe(p->inputPipeId1), *GetPipe(p->outputPipeId), p->keepMe, p->numAttsInput,
                         p->numAttsOutput);
            break;
        }

        case DUPLICATE_REMOVAL: {
            DuplicateRemovalPlanNode *d = dynamic_cast<DuplicateRemovalPlanNode *>(planNode);
            DuplicateRemoval *duplicateRemoval = new DuplicateRemoval();
            duplicateRemoval->Run(*GetPipe(d->inputPipeId1), *GetPipe(d->outputPipeId), *d->inputSchema);
            break;
        }
    }
}

void QueryRunner::OutputQueryResult() {

    Pipe *finalOutPipe = GetPipe(planNode->outputPipeId);

    if (queryOutputType == STD_OUT) {
        Record temp;
        cout << "\n";
        while (finalOutPipe->Remove(&temp)) {
            temp.Print(planNode->outputSchema);
        }
        return;
    }
    if (queryOutputType == FILE_OUT) {
        WriteOut writeOut;
        FILE *writefile = fopen(outputFileName, "w");
        writeOut.Run(*finalOutPipe, writefile, *planNode->outputSchema);
        writeOut.WaitUntilDone();
    }

}

Pipe *QueryRunner::GetPipe(int pipeId) {
    if (pipes.find(pipeId) == pipes.end()) {
        pipes[pipeId] = new Pipe(PIPE_BUFFER_SIZE);
    }
    return pipes[pipeId];
}