#ifndef QUERY_PLAN_NODE_H
#define QUERY_PLAN_NODE_H

#include "RelOpPlanNode.h"

RelOpPlanNode::RelOpPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) {
    this->child1 = child1;
    this->child2 = child2;

    if (this->child1) {
        this->inputPipeId1 = this->child1->outputPipeId;
    }

    if (this->child2) {
        this->inputPipeId2 = this->child2->outputPipeId;
    }
}

void RelOpPlanNode::Print() {
    if (inputPipeId1 != -1) {
        cout << "Input Pipe " << inputPipeId1 << "\n";
    }

    if (inputPipeId2 != -1) {
        cout << "Input Pipe " << inputPipeId2 << "\n";
    }

    if (outputPipeId != -1) {
        cout << "Output Pipe " << outputPipeId << "\n";
    }

    if (outputSchema) {
        cout << "Output Schema:" << "\n";
        outputSchema->Print();
    }
}

DuplicateRemovalPlanNode::DuplicateRemovalPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) : RelOpPlanNode(child1,
                                                                                                                 child2) {
    this->type = DUPLICATE_REMOVAL;
}

void DuplicateRemovalPlanNode::Print() {
    cout << " *********** " << "\n";
    cout << "DISTINCT operation" << "\n";
    RelOpPlanNode::Print();
}

GroupByPlanNode::GroupByPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) : RelOpPlanNode(child1, child2) {
    this->type = GROUP_BY;
}

void GroupByPlanNode::Print() {
    cout << " *********** " << "\n";
    cout << "GROUP BY operation" << "\n";
    RelOpPlanNode::Print();
    cout << "\n";

    if (groupAtts) {
        cout << "GROUPING ON" << "\n";
        groupAtts->Print(child1->outputSchema);
        cout << "\n";
    }

    if (computeMe) {
        cout << "FUNCTION" << "\n";
        computeMe->Print(child1->outputSchema);
        cout << "\n";
        cout << "Distinct Function: " << distinctFunc << "\n";
        cout << "\n";
    }
}

JoinPlanNode::JoinPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) : RelOpPlanNode(child1, child2) {
    this->type = JOIN;
}

void JoinPlanNode::Print() {
    cout << " *********** " << "\n";
    cout << "JOIN operation" << "\n";
    RelOpPlanNode::Print();
    cout << "\n";

    cout << "CNF: " << "\n";
    if (selOp) {
        selOp->Print(child1->outputSchema, child2->outputSchema, literal);
        cout << "\n";
    }

}

SelectFilePlanNode::SelectFilePlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) : RelOpPlanNode(child1, child2) {
    this->type = SELECT_FILE;
}

void SelectFilePlanNode::Print() {
    cout << " *********** " << "\n";
    cout << "SELECT FILE operation" << "\n";
    RelOpPlanNode::Print();
    cout << "\n";

    if (selOp) {
        cout << "SELECTION CNF :" << "\n";
        selOp->Print(outputSchema, NULL, literal);
        cout << "\n";
    }
}

SelectPipePlanNode::SelectPipePlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) : RelOpPlanNode(child1, child2) {
    this->type = SELECT_PIPE;
}

void SelectPipePlanNode::Print() {
    cout << " *********** " << "\n";
    cout << "SELECT PIPE operation" << "\n";
    RelOpPlanNode::Print();
    cout << "\n";

    if (selOp) {
        cout << "SELECTION CNF :" << "\n";
        selOp->Print(child1->outputSchema, NULL, literal);
        cout << "\n";
    }
}

SumPlanNode::SumPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) : RelOpPlanNode(child1, child2) {
    this->type = SUM;
}

void SumPlanNode::Print() {
    cout << " *********** " << "\n";
    cout << "SUM operation" << "\n";
    RelOpPlanNode::Print();
    cout << "\n";

    if (computeMe) {
        cout << "FUNCTION" << "\n";
        computeMe->Print(child1->outputSchema);
        cout << "\n";
        cout << "Distinct Function: " << distinctFunc << "\n";
        cout << "\n";
    }
}

ProjectPlanNode::ProjectPlanNode(RelOpPlanNode *child1, RelOpPlanNode *child2) : RelOpPlanNode(child1, child2) {
    this->type = PROJECT;
}

void ProjectPlanNode::Print() {
    cout << " *********** " << "\n";
    cout << "PROJECT operation" << "\n";
    RelOpPlanNode::Print();
    cout << "Number of attributes Input: " << numAttsInput << "\n";
    cout << "Number of attributes Output: " << numAttsOutput << "\n";
    cout << "Keep Me: ";
    for (int i = 0; i < numAttsOutput; i++) {
        cout << keepMe[i] << " ";
    }
    cout << "\n";
}

#endif


