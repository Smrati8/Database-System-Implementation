#ifndef STATISTICS_
#define STATISTICS_

#include "ParseTree.h"

#include <cstring>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include<sstream>

using namespace std;

struct Relation {
private:
    double numOfTuples;

public:
    Relation() {
        this->numOfTuples = 0;
    }

    Relation(double numOfTuples) {
        this->numOfTuples = numOfTuples;
    }

    double GetNumOfTuples() {
        return numOfTuples;
    }

    void SetNumOfTuples(double n) {
        this->numOfTuples = n;
    }
};

struct Att {
private:
    int numOfDistinct;

public:
    Att() {
        this->numOfDistinct = 0;
    }

    Att(int numOfDistinct) {
        this->numOfDistinct = numOfDistinct;
    }

    int GetNumOfDistinct() {
        return numOfDistinct;
    }

    void SetNumOfDistinct(int n) {
        this->numOfDistinct = n;
    }
};

class Statistics {

private:
    unordered_map<string, Relation> groupNameToRelationMap;
    unordered_map<string, Att> attNameToAttributeMap;
    unordered_map<string, unordered_set<string> > groupNameToSetOfRelationsMap;
    unordered_map<string, string> relNameToGroupNameMap;

    void AddAtt(const string &relName, string attName, int numDistincts);

    void PreProcessApply(struct AndList *parseTree, unordered_set<string> *relNames);

    void ValidateApplyOnRelations(unordered_set<string> *relNames);

    void PreProcessApplyOnAttributes(struct AndList *parseTree, unordered_set<string> *relNames);

    void PreProcessNameOperand(Operand *operand, unordered_set<string> *relNames);

public:
    Statistics();

    Statistics(Statistics &copyMe);     // Performs deep copy

    ~Statistics();

    void AddRel(char *relName, int numTuples);

    void AddAtt(char *relName, char *attName, int numDistincts);

    void CopyRel(char *oldName, char *newName);

    void Read(char *fromWhere);

    void Write(char *fromWhere);

    void Apply(struct AndList *parseTree, char *relNames[], int numToJoin);

    double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);

    unordered_map<string, Relation> *GetGroupNameToRelationMap();

    unordered_map<string, Att> *GetAttNameToAttributeMap();

    unordered_map<string, unordered_set<string> > *GetGroupNameToSetOfRelationsMap();

    unordered_map<string, string> *GetRelNameToGroupNameMap();
};

#endif
