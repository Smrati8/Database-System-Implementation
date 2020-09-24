#include <iostream>

#include "../ParseTree.h"
#include "../QueryPlan.h"
#include "../Statistics.h"

using namespace std;

// test settings file should have the
// catalog_path
const char *settings = "test.cat";

// donot change this information here
char *catalog_path = NULL;

extern "C" {
int yyparse(void);   // defined in y.tab.c
}

extern struct FuncOperator *finalFunction;
extern struct TableList *tables;
extern struct AndList *boolean;
extern struct NameList *groupingAtts;
extern struct NameList *attsToSelect;
extern int distinctAtts;
extern int distinctFunc;

char *statisticsFileName = "Statistics-42.txt";

void setup () {
    FILE *fp = fopen (settings, "r");
    if (fp) {
        char *mem = (char *) malloc(80);
        catalog_path = &mem[0];
        char line[80];
        fgets (line, 80, fp);
        sscanf (line, "%s\n", catalog_path);

        if (!catalog_path) {
            cerr << " Test settings file 'test.cat' not in correct format.\n";
            free (mem);
            exit (1);
        }
    } else {
        cerr << " Test settings files 'test.cat' missing \n";
        exit (1);
    }
}

int main() {
    setup();

    // Loading Statistics from the file.
    Statistics statistics;
    statistics.Read(statisticsFileName);

    // Parse the query.
    yyparse();

    Query query = {finalFunction, tables, boolean, groupingAtts, attsToSelect, distinctAtts, distinctFunc};

    // Create query plan/
    QueryPlan queryPlan(catalog_path, &statistics, &query);

    // Print the query plan in post order.
    queryPlan.Print();
}


