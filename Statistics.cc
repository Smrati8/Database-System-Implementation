#include "Statistics.h"

Statistics::Statistics() {
}

Statistics::Statistics(Statistics &copyMe) {
    for (auto &x : copyMe.groupNameToRelationMap) {
        groupNameToRelationMap[x.first] = Relation(x.second.GetNumOfTuples());
    }

    for (auto &attNameToAttributeMapItem : copyMe.attNameToAttributeMap) {
        attNameToAttributeMap[attNameToAttributeMapItem.first] =
                Att(attNameToAttributeMapItem.second.GetNumOfDistinct());
    }

    for (auto &setNameToSetOfRelationsMapItem : copyMe.groupNameToSetOfRelationsMap) {
        unordered_set<string> newRelationSet;
        for (auto &relName : setNameToSetOfRelationsMapItem.second) {
            newRelationSet.insert(relName);
        }
        groupNameToSetOfRelationsMap[setNameToSetOfRelationsMapItem.first] = newRelationSet;
    }

    for (auto &relNameToSetNameMapItem : copyMe.relNameToGroupNameMap) {
        relNameToGroupNameMap[relNameToSetNameMapItem.first] = relNameToSetNameMapItem.second;
    }
}

Statistics::~Statistics() {
}

void Statistics::AddRel(char *relName, int numTuples) {

    // If the relation is not present. Add new relation and corresponding entries in map.
    if (groupNameToSetOfRelationsMap.find(relName) == groupNameToSetOfRelationsMap.end()) {
        unordered_set<string> newRelationSet;
        newRelationSet.insert(relName);

        relNameToGroupNameMap[relName] = relName;
        groupNameToSetOfRelationsMap[relName] = newRelationSet;
        groupNameToRelationMap[relName] = Relation(numTuples);

        // If the relation is not yet joined, update the number of tuples.
    } else if (relNameToGroupNameMap[relName] == relName) {
        groupNameToRelationMap[relName].SetNumOfTuples(numTuples);

        // Otherwise throw an error, as table is already joined.
    } else {
        cerr << "Relation is already joined with some table.\n";
        exit(1);
    }
}

void Statistics::AddAtt(char *relName, char *attName, int numDistincts) {
    AddAtt(string(relName), string(attName), numDistincts);
}

void Statistics::AddAtt(const string &relName, string attName, int numDistincts) {
    string attNameWithRelName = relName + "." + attName;

    if (attNameToAttributeMap.find(attNameWithRelName) != attNameToAttributeMap.end()
        && relNameToGroupNameMap[relName] != relName) {
        cerr << "Relation is already joined with some table. Hence attribute can't be updated.\n";
        exit(1);
    }

    if (numDistincts == -1) {
        numDistincts = groupNameToRelationMap[relNameToGroupNameMap[relName]].GetNumOfTuples();
    }
    attNameToAttributeMap[attNameWithRelName] = Att(numDistincts);
}

void Statistics::CopyRel(char *oldName, char *newName) {

    // Add new relation.
    AddRel(newName, groupNameToRelationMap[relNameToGroupNameMap[oldName]].GetNumOfTuples());

    // Add attributes in the new relation.
    for (auto attNameToAttributeMapItem : attNameToAttributeMap) {
        string attNameWithRelName = attNameToAttributeMapItem.first;
        string relName = attNameWithRelName.substr(0, attNameWithRelName.find('.'));

        if (relName == string(oldName)) {
            string attName = attNameWithRelName.substr(attNameWithRelName.find('.') + 1);
            AddAtt(string(newName), attName, attNameToAttributeMapItem.second.GetNumOfDistinct());
        }
    }
}

void Statistics::Read(char *fromWhere) {
    ifstream fIn;
    fIn.open(fromWhere);

    if (!fIn.is_open()) return;

    string readLine;

    getline(fIn, readLine);
    getline(fIn, readLine);
    int setNameToRelationMapSize = stoi(readLine);
    groupNameToRelationMap.clear();
    for (int i = 0; i < setNameToRelationMapSize; i++) {
        getline(fIn, readLine, '=');
        string groupName = readLine;
        getline(fIn, readLine);
        int numOfTuples = stoi(readLine);
        groupNameToRelationMap[groupName] = Relation(numOfTuples);
    }

    getline(fIn, readLine);
    getline(fIn, readLine);
    int attNameToAttributeMapSize = stoi(readLine);
    attNameToAttributeMap.clear();
    for (int i = 0; i < attNameToAttributeMapSize; i++) {
        getline(fIn, readLine, '=');
        string attName = readLine;
        getline(fIn, readLine);
        int numOfDistinct = stoi(readLine);
        attNameToAttributeMap[attName] = Att(numOfDistinct);
    }

    getline(fIn, readLine);
    getline(fIn, readLine);
    int setNameToSetOfRelationsMapSize = stoi(readLine);
    groupNameToSetOfRelationsMap.clear();
    for (int i = 0; i < setNameToSetOfRelationsMapSize; i++) {
        getline(fIn, readLine, '=');
        string groupName = readLine;

        unordered_set<string> newRelationSet;
        groupNameToSetOfRelationsMap[groupName] = newRelationSet;

        getline(fIn, readLine);
        stringstream s_stream(readLine);

        while (s_stream.good()) {
            getline(s_stream, readLine, ',');
            groupNameToSetOfRelationsMap[groupName].insert(readLine);
        }
    }

    getline(fIn, readLine);
    getline(fIn, readLine);
    int relNameToSetNameMapSize = stoi(readLine);
    relNameToGroupNameMap.clear();
    for (int i = 0; i < attNameToAttributeMapSize; i++) {
        getline(fIn, readLine, '=');
        string relName = readLine;
        getline(fIn, readLine);
        string groupName = readLine;
        relNameToGroupNameMap[relName] = groupName;
    }

}

void Statistics::Write(char *fromWhere) {
    ofstream fOut;
    fOut.open(fromWhere);

    fOut << "**************** Group Relations *****************\n";
    fOut << groupNameToRelationMap.size() << "\n";
    for (auto &x: groupNameToRelationMap) {
        fOut << x.first << "=" << x.second.GetNumOfTuples() << "\n";
    }

    fOut << "**************** Attributes ******************\n";
    fOut << attNameToAttributeMap.size() << "\n";
    for (auto &x: attNameToAttributeMap) {
        fOut << x.first << "=" << x.second.GetNumOfDistinct() << "\n";
    }

    fOut << "****************** GroupName to Relations ****************\n";
    fOut << groupNameToSetOfRelationsMap.size() << "\n";
    for (auto &x: groupNameToSetOfRelationsMap) {
        auto secondIterator = x.second.begin();
        fOut << x.first << "=" << *(secondIterator);
        while (++secondIterator != x.second.end()) {
            fOut << "," << *(secondIterator);
        }
        fOut << "\n";
    }

    fOut << "******************** Relation Name to Group Name *****************\n";
    fOut << relNameToGroupNameMap.size() << "\n";
    for (auto &x: relNameToGroupNameMap) {
        fOut << x.first << "=" << x.second << "\n";
    }
}

void Statistics::Apply(struct AndList *parseTree, char *relNames[], int numToJoin) {
    unordered_set<string> relNamesSet;
    for (int i = 0; i < numToJoin; i++) {
        relNamesSet.insert(relNames[i]);
    }

    PreProcessApply(parseTree, &relNamesSet);
    string resultantGroupName;
    unordered_map<string, double> attNameToProbabilitiesMap;
    while (parseTree) {
        attNameToProbabilitiesMap.clear();

        OrList *orList = parseTree->left;
        while (orList) {
            ComparisonOp *currentComparisonOp = orList->left;
            Operand *leftOperand = currentComparisonOp->left;
            Operand *rightOperand = currentComparisonOp->right;
            int comparisonOperator = currentComparisonOp->code;

            // if both side of a operator, there is a name, then Join the two tables.
            if (leftOperand->code == NAME && rightOperand->code == NAME) {

                if (comparisonOperator != EQUALS) {
                    cerr << "Join is not implemented for other than Equals operator\n";
                    exit(1);
                }

                string leftAttNameWithRelName = string(leftOperand->value);
                int numOfDistinctInLeftAtt = attNameToAttributeMap[leftAttNameWithRelName].GetNumOfDistinct();
                string leftRelName = leftAttNameWithRelName.substr(0, leftAttNameWithRelName.find('.'));
                string leftGroupName = relNameToGroupNameMap[leftRelName];
                double numOfTuplesInLeftGroup = groupNameToRelationMap[leftGroupName].GetNumOfTuples();

                string rightAttNameWithRelName = string(rightOperand->value);
                int numOfDistinctInRightAtt = attNameToAttributeMap[rightAttNameWithRelName].GetNumOfDistinct();
                string rightRelName = rightAttNameWithRelName.substr(0, rightAttNameWithRelName.find('.'));
                string rightGroupName = relNameToGroupNameMap[rightRelName];
                double numOfTuplesInRightGroup = groupNameToRelationMap[rightGroupName].GetNumOfTuples();

                if (leftGroupName == rightGroupName) {
                    cerr << "Table " << leftRelName << " is already joined with " << rightGroupName << ".\n";
                    exit(1);
                }

                double numOfTuplesPerAttValueInLeft = (numOfTuplesInLeftGroup / numOfDistinctInLeftAtt);
                double numOfTuplesPerAttValueInRight = (numOfTuplesInRightGroup / numOfDistinctInRightAtt);

                double numOfTuplesAfterJoin = numOfTuplesPerAttValueInLeft
                                              * numOfTuplesPerAttValueInRight
                                              * min(numOfDistinctInLeftAtt, numOfDistinctInRightAtt);

                string newGroupName;
                newGroupName.append(leftGroupName).append("&").append(rightGroupName);

                // Delete leftGroups and rightGroups for Different map.
                groupNameToRelationMap.erase(leftGroupName);
                groupNameToRelationMap.erase(rightGroupName);

                // Create new group relation.
                groupNameToRelationMap[newGroupName] = numOfTuplesAfterJoin;
                unordered_set<string> newRelationSet;


                // Change groups of leftGroups and rightGroups relations.
                for (const string &relName : groupNameToSetOfRelationsMap[leftGroupName]) {
                    relNameToGroupNameMap[relName] = newGroupName;
                    newRelationSet.insert(relName);
                }
                groupNameToSetOfRelationsMap.erase(leftGroupName);

                for (const string &relName : groupNameToSetOfRelationsMap[rightGroupName]) {
                    relNameToGroupNameMap[relName] = newGroupName;
                    newRelationSet.insert(relName);
                }
                groupNameToSetOfRelationsMap.erase(rightGroupName);

                groupNameToSetOfRelationsMap[newGroupName] = newRelationSet;
                resultantGroupName = newGroupName;
            }
                // Otherwise it is a select operation.
            else if (leftOperand->code == NAME ^ rightOperand->code == NAME) {
                Operand *nameOperand = leftOperand->code == NAME ? leftOperand : rightOperand;
                string attNameWithRelName = string(nameOperand->value);
                string relName = attNameWithRelName.substr(0, attNameWithRelName.find('.'));
                if (currentComparisonOp->code == EQUALS) {
                    double probabilityFraction = 1.0 / attNameToAttributeMap[attNameWithRelName].GetNumOfDistinct();
                    if (attNameToProbabilitiesMap.find(attNameWithRelName) == attNameToProbabilitiesMap.end()) {
                        attNameToProbabilitiesMap[attNameWithRelName] = probabilityFraction;
                    } else {
                        attNameToProbabilitiesMap[attNameWithRelName] += probabilityFraction;
                    }
                } else {
                    if (attNameToProbabilitiesMap.find(attNameWithRelName) == attNameToProbabilitiesMap.end()) {
                        attNameToProbabilitiesMap[attNameWithRelName] = (1.0 / 3.0);
                    } else {

                    }
                }
                resultantGroupName = relNameToGroupNameMap[relName];
            } else {
                cerr << "left operand " << string(leftOperand->value) << " and right operand "
                     << string(rightOperand->value) << " are not valid.\n";
                exit(1);
            }
            orList = orList->rightOr;
        }

        if (!attNameToProbabilitiesMap.empty()) {
            double numOfTuples = groupNameToRelationMap[resultantGroupName].GetNumOfTuples();
            double multiplicationFactor = 0.0;

            if (attNameToProbabilitiesMap.size() == 1) {
                multiplicationFactor = (*attNameToProbabilitiesMap.begin()).second;
            } else {
                double additionFactor = 0.0;
                double subtractionFactor = 1.0;

                for (const auto &attNameToProbabilitiesMapItem : attNameToProbabilitiesMap) {
                    additionFactor += attNameToProbabilitiesMapItem.second;
                    subtractionFactor *= attNameToProbabilitiesMapItem.second;
                }
                multiplicationFactor = additionFactor - subtractionFactor;

            }

            numOfTuples *= multiplicationFactor;


            groupNameToRelationMap[resultantGroupName].SetNumOfTuples(numOfTuples);
        }
        parseTree = parseTree->rightAnd;
    }

}

double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin) {
    Statistics dummy(*this);

    dummy.Apply(parseTree, relNames, numToJoin);
    unordered_set<string> groupNames;
    for (int i = 0; i < numToJoin; i++) {
        groupNames.insert(dummy.relNameToGroupNameMap[relNames[i]]);
    }

    if (groupNames.size() != 1) {
        cerr << "Error while estimating.\n";
        exit(1);
    }

    return dummy.groupNameToRelationMap[*groupNames.begin()].GetNumOfTuples();
}

void Statistics::PreProcessApply(struct AndList *parseTree, unordered_set<string> *relNames) {
    // Validate Relations
    ValidateApplyOnRelations(relNames);
    // Check attributes
    PreProcessApplyOnAttributes(parseTree, relNames);
}

void Statistics::ValidateApplyOnRelations(unordered_set<string> *relNames) {
    unordered_set<string> setNamesToJoin;
    for (const string &relName : *relNames) {
        if (relNameToGroupNameMap.find(relName) == relNameToGroupNameMap.end()) {
            cerr << "Relation " << relName << " is not present in statistics.\n";
            exit(1);
        }
        setNamesToJoin.insert(relNameToGroupNameMap[relName]);
    }

    unordered_set<string> relationsInResult;
    for (const string &setName : setNamesToJoin) {
        for (const string &relName : groupNameToSetOfRelationsMap[setName]) {
            relationsInResult.insert(relName);
        }
    }

    for (const string &relName : *relNames) {
        relationsInResult.erase(relName);
    }

    if (!relationsInResult.empty()) {
        cerr << "Relation association doesn't make sense\n";
        exit(1);
    }
}

void Statistics::PreProcessApplyOnAttributes(struct AndList *parseTree, unordered_set<string> *relNames) {
    while (parseTree) {
        OrList *orList = parseTree->left;
        while (orList) {
            if (orList->left->left->code == NAME) {
                PreProcessNameOperand(orList->left->left, relNames);
            }
            if (orList->left->right->code == NAME) {
                PreProcessNameOperand(orList->left->right, relNames);
            }
            orList = orList->rightOr;
        }
        parseTree = parseTree->rightAnd;
    }
}

void Statistics::PreProcessNameOperand(Operand *operand, unordered_set<string> *relNames) {
    string operandValue = operand->value;

    // If operandValue contains relation name i.e name is of the form "relName.attName".
    if (operandValue.find('.') != string::npos) {
        string relationName = operandValue.substr(0, operandValue.find('.'));
        if (attNameToAttributeMap.find(operandValue) == attNameToAttributeMap.end()) {
            cerr << "Attribute " << string(operandValue) << " is not present in Statistics.\n";
        }
        if (relNames->find(relationName) == relNames->end()) {
            cerr << "Attribute is not linked with any rel names given.\n";
        }
    } else {
        bool relFound = false;
        for (const string &relName : *relNames) {
            string attributeNameWithRelName = relName + "." + string(operandValue);
            if (attNameToAttributeMap.find(attributeNameWithRelName) != attNameToAttributeMap.end()) {
                relFound = true;
                char *newOperandValue = new char[attributeNameWithRelName.size() + 1];
                strcpy(newOperandValue, attributeNameWithRelName.c_str());
                operand->value = newOperandValue;
                break;
            }
        }
        if (!relFound) {
            cerr << "No relation contains attribute " << operandValue << ".\n";
            exit(1);
        }
    }
}

unordered_map<string, Relation> *Statistics::GetGroupNameToRelationMap() {
    return &this->groupNameToRelationMap;
}

unordered_map<string, Att> *Statistics::GetAttNameToAttributeMap() {
    return &this->attNameToAttributeMap;
}

unordered_map<string, unordered_set<string> > *Statistics::GetGroupNameToSetOfRelationsMap() {
    return &this->groupNameToSetOfRelationsMap;
}

unordered_map<string, string> *Statistics::GetRelNameToGroupNameMap() {
    return &this->relNameToGroupNameMap;
}