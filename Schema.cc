#include "Schema.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int Schema::Find(char *attName) {

    for (int i = 0; i < numAtts; i++) {
        if (!strcmp(attName, myAtts[i].name)) {
            return i;
        }
    }

    // if we made it here, the attribute was not found
    return -1;
}

Type Schema::FindType(char *attName) {

    for (int i = 0; i < numAtts; i++) {
        if (!strcmp(attName, myAtts[i].name)) {
            return myAtts[i].myType;
        }
    }

    // if we made it here, the attribute was not found
    return Int;
}

int Schema::GetNumAtts() {
    return numAtts;
}

Attribute *Schema::GetAtts() {
    return myAtts;
}

Schema::Schema() {
    numAtts = 0;
    myAtts = NULL;
}

Schema::Schema(OrderMaker &order) {
    numAtts = order.numAtts;
    myAtts = new Attribute[numAtts];
    for (int i = 0; i < numAtts; i++) {
        myAtts[i].myType = order.whichTypes[i];
        myAtts[i].name = reinterpret_cast<char *>(order.whichAtts[i]);
    }
}

Schema::Schema(char *fpath, int num_atts, Attribute *atts) {
    fileName = strdup(fpath);
    numAtts = num_atts;
    myAtts = new Attribute[numAtts];
    for (int i = 0; i < numAtts; i++) {
        if (atts[i].myType == Int) {
            myAtts[i].myType = Int;
        } else if (atts[i].myType == Double) {
            myAtts[i].myType = Double;
        } else if (atts[i].myType == String) {
            myAtts[i].myType = String;
        } else {
            cout << "Bad attribute type for " << atts[i].myType << "\n";
            delete[] myAtts;
            exit(1);
        }
        myAtts[i].name = strdup(atts[i].name);
    }
}

Schema::Schema(char *fName, char *relName) {

    FILE *foo = fopen(fName, "r");

    // this is enough space to hold any tokens
    char space[200];

    fscanf(foo, "%s", space);
    int totscans = 1;

    // see if the file starts with the correct keyword
    if (strcmp(space, "BEGIN")) {
        cout << "Unfortunately, this does not seem to be a schema file.\n";
        exit(1);
    }

    while (1) {

        // check to see if this is the one we want
        fscanf(foo, "%s", space);
        totscans++;
        if (strcmp(space, relName)) {

            // it is not, so suck up everything to past the BEGIN
            while (1) {

                // suck up another token
                if (fscanf(foo, "%s", space) == EOF) {
                    cerr << "Could not find the schema for the specified relation.\n";
                    exit(1);
                }

                totscans++;
                if (!strcmp(space, "BEGIN")) {
                    break;
                }
            }

            // otherwise, got the correct file!!
        } else {
            break;
        }
    }

    // suck in the file name
    fscanf(foo, "%s", space);
    totscans++;
    fileName = strdup(space);

    // count the number of attributes specified
    numAtts = 0;
    while (1) {
        fscanf(foo, "%s", space);
        if (!strcmp(space, "END")) {
            break;
        } else {
            fscanf(foo, "%s", space);
            numAtts++;
        }
    }

    // now actually load up the schema
    fclose(foo);
    foo = fopen(fName, "r");

    // go past any un-needed info
    for (int i = 0; i < totscans; i++) {
        fscanf(foo, "%s", space);
    }

    // and load up the schema
    myAtts = new Attribute[numAtts];
    for (int i = 0; i < numAtts; i++) {

        // read in the attribute name
        fscanf(foo, "%s", space);
        myAtts[i].name = strdup(space);

        // read in the attribute type
        fscanf(foo, "%s", space);
        if (!strcmp(space, "Int")) {
            myAtts[i].myType = Int;
        } else if (!strcmp(space, "Double")) {
            myAtts[i].myType = Double;
        } else if (!strcmp(space, "String")) {
            myAtts[i].myType = String;
        } else {
            cout << "Bad attribute type for " << myAtts[i].name << "\n";
            exit(1);
        }
    }

    fclose(foo);
}

Schema::Schema(Schema *schema1, Schema *schema2) {
    int numAtts1 = schema1->numAtts;
    int numAtts2 = schema2->numAtts;
    numAtts = numAtts1 + numAtts2;
    myAtts = new Attribute[numAtts];

    for (int i = 0; i < numAtts1; i++) {
        myAtts[i] = schema1->myAtts[i];
    }

    for (int i = 0; i < numAtts2; i++) {
        myAtts[numAtts1 + i] = schema2->myAtts[i];
    }
}

Schema::Schema(Schema *baseSchema, NameList *nameList, vector<int> *keepMeVector) {
    numAtts = 0;
    NameList *nameListLocal = nameList;
    while (nameListLocal) {
        numAtts++;
        nameListLocal = nameListLocal->next;
    }

    myAtts = new Attribute[numAtts];
    int *keepAttsPos = new int[numAtts];
    int i = 0;

    nameListLocal = nameList;
    while (nameListLocal) {
        int pos = baseSchema->Find(nameListLocal->name);
        if (pos == -1) {
            cerr << "Attribute " + string(nameListLocal->name) + " is not present in the base relation\n";
            exit(1);
        }

        keepMeVector->push_back(pos);
        myAtts[i++] = baseSchema->myAtts[pos];

        nameListLocal = nameListLocal->next;
    }
}

void Schema::AliasAttributes(std::string aliasName) {
    // this is enough space to hold any tokens
    for (int i = 0; i < numAtts; i++) {
        string newAttName = aliasName + "." + string(myAtts[i].name);
        char *newAttNameChar = new char[200];
        strcpy(newAttNameChar, newAttName.c_str());
        myAtts[i].name = newAttNameChar;
    }
}

void Schema::Write(char *fileName, char *tableName) {
    ofstream fOut;
    fOut.open(fileName);
    fOut << "BEGIN\n";
    fOut << tableName << "\n";
    fOut << tableName << ".tbl\n";
    for (int i = 0; i < numAtts; i++) {
        switch (myAtts[i].myType) {
            case Int:
                fOut << myAtts[i].name << " Int\n";
                break;
            case Double:
                fOut << myAtts[i].name << " Double\n";
                break;
            case String:
                fOut << myAtts[i].name << " String\n";
                break;
        }
    }
    fOut << "END\n";
    fOut.close();
}

void Schema::Print() {
    for (int i = 0; i < numAtts; i++) {
        cout << "\t" << "Att " + string(myAtts[i].name) + ": ";
        switch (myAtts[i].myType) {
            case Int:
                cout << "INT" << "\n";
                break;
            case Double:
                cout << "DOUBLE" << "\n";
                break;
            case String:
                cout << "STRING" << "\n";
                break;
        }
    }
}

Schema::~Schema() {
    delete[] myAtts;
    myAtts = 0;
}

