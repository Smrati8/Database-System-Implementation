 
%{

	#include "ParseTree.h" 
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <iostream>

	extern "C" int yylex();
	extern "C" int yyparse();
	extern "C" void yyerror(char *s);
  
	// these data structures hold the result of the parsing
	QueryType queryType; // type of query
	OutputType outputType;
	struct NameList *createAtts; // attributes in create query.
	struct NameList *sortAtts; // sort attributes in sorted file creation.
	FileType fileType; // HEAP or SORTED file inside create operation.
	char *fileName; // load file name given in Insert query.
	struct FuncOperator *finalFunction; // the aggregate function (NULL if no agg)
	struct TableList *tables; // the list of tables and aliases in the query
	struct AndList *boolean; // the predicate in the WHERE clause
	struct NameList *groupingAtts; // grouping atts (NULL if no grouping)
	struct NameList *attsToSelect; // the set of attributes in the SELECT (NULL if no such atts)
	int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query 
	int distinctFunc;  // 1 if there is a DISTINCT in an aggregate query

%}

// this stores all of the types returned by production rules
%union {
 	struct FuncOperand *myOperand;
	struct FuncOperator *myOperator; 
	struct TableList *myTables;
	struct ComparisonOp *myComparison;
	struct Operand *myBoolOperand;
	struct OrList *myOrList;
	struct AndList *myAndList;
	struct NameList *myNames;
	char *actualChars;
	char whichOne;
}

%token <actualChars> Name
%token <actualChars> Float
%token <actualChars> Int
%token <actualChars> String
%token CREATE_TOKEN
%token INSERT_TOKEN
%token DROP_TOKEN
%token SELECT_TOKEN
%token TABLE
%token FROM
%token WHERE
%token INTO
%token SUM
%token GROUP
%token AND
%token DISTINCT
%token BY
%token OR
%token AS
%token ON
%token HEAP_TOKEN
%token SORTED_TOKEN
%token INTEGER_TOKEN
%token DOUBLE_TOKEN
%token STRING_TOKEN
%token SET_TOKEN
%token OUTPUT
%token STDOUT
%token NONE

%type <myOrList> OrList
%type <myAndList> AndList
%type <myOperand> SimpleExp
%type <myOperator> CompoundExp
%type <whichOne> Op 
%type <myComparison> BoolComp
%type <myComparison> Condition
%type <myTables> Tables Table
%type <myBoolOperand> Literal
%type <myNames> Atts AttsWithType

%start SQL


//******************************************************************************
// SECTION 3
//******************************************************************************
/* This is the PRODUCTION RULES section which defines how to "understand" the 
 * input language and what action to take for each "statment"
 */

%%

SQL: SET_TOKEN OUTPUT STDOUT ';'
{
	queryType = SET;
	outputType = STD_OUT;
}

| SET_TOKEN OUTPUT STDOUT
{
	queryType = SET;
	outputType = STD_OUT;
}

| SET_TOKEN OUTPUT NONE ';'
{
	queryType = SET;
	outputType = NO_OUT;
}

| SET_TOKEN OUTPUT NONE
{
	queryType = SET;
	outputType = NO_OUT;
}

| SET_TOKEN OUTPUT String ';'
{
	queryType = SET;
	outputType = FILE_OUT;
	fileName = $3;
}

| SET_TOKEN OUTPUT String {
	queryType = SET;
	outputType = FILE_OUT;
	fileName = $3;
}

| CREATE_TOKEN TABLE Table '('AttsWithType')' AS HEAP_TOKEN ';'
{
	queryType = CREATE;
	tables = $3;
	createAtts = $5;
	fileType = HEAP;
}

| CREATE_TOKEN TABLE Table '('AttsWithType')' AS HEAP_TOKEN
{
	queryType = CREATE;
	tables = $3;
	createAtts = $5;
	fileType = HEAP;
}

| CREATE_TOKEN TABLE Table '('AttsWithType')' AS SORTED_TOKEN ON Atts ';'
{
	queryType = CREATE;
	tables = $3;
        createAtts = $5;
	sortAtts = $10;
	fileType = SORTED;
}

| CREATE_TOKEN TABLE Table '('AttsWithType')' AS SORTED_TOKEN ON Atts
{
	queryType = CREATE;
	tables = $3;
        createAtts = $5;
	sortAtts = $10;
	fileType = SORTED;
}

| INSERT_TOKEN String INTO Table ';'
{
	queryType = INSERT;
	fileName = $2;
	tables = $4;
}

| INSERT_TOKEN String INTO Table
{
	queryType = INSERT;
	fileName = $2;
	tables = $4;
}

| DROP_TOKEN TABLE Table ';'
{
	queryType = DROP;
	tables = $3;
}

| DROP_TOKEN TABLE Table
{
	queryType = DROP;
	tables = $3;
}

| SELECT_TOKEN WhatIWant FROM Tables WHERE AndList
{
	queryType = SELECT;
	tables = $4;
	boolean = $6;
	groupingAtts = NULL;
}

| SELECT_TOKEN WhatIWant FROM Tables WHERE AndList GROUP BY Atts
{
	queryType = SELECT;
	tables = $4;
	boolean = $6;	
	groupingAtts = $9;
};

WhatIWant: Function ',' Atts 
{
	attsToSelect = $3;
	distinctAtts = 0;
}

| Function
{
	attsToSelect = NULL;
}

| Atts 
{
	distinctAtts = 0;
	finalFunction = NULL;
	attsToSelect = $1;
}

| DISTINCT Atts
{
	distinctAtts = 1;
	finalFunction = NULL;
	attsToSelect = $2;
	finalFunction = NULL;
};

Function: SUM '(' CompoundExp ')'
{
	distinctFunc = 0;
	finalFunction = $3;
}

| SUM DISTINCT '(' CompoundExp ')'
{
	distinctFunc = 1;
	finalFunction = $4;
};

Atts: Name
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->next = NULL;
} 

| Atts ',' Name
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $3;
	$$->next = $1;
};

AttsWithType:
| Name INTEGER_TOKEN
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->type = INT;
	$$->next = NULL;
}
| Name DOUBLE_TOKEN
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->type = DOUBLE;
	$$->next = NULL;
}
| Name STRING_TOKEN
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->type = STRING;
	$$->next = NULL;
}
| Name INTEGER_TOKEN ',' AttsWithType
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->type = INT;
	$$->next = $4;
}
| Name DOUBLE_TOKEN ',' AttsWithType
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->type = DOUBLE;
	$$->next = $4;
}
| Name STRING_TOKEN ',' AttsWithType
{
	$$ = (struct NameList *) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->type = STRING;
	$$->next = $4;
};

Tables: Name AS Name 
{
	$$ = (struct TableList *) malloc (sizeof (struct TableList));
	$$->tableName = $1;
	$$->aliasAs = $3;
	$$->next = NULL;
}

| Tables ',' Name AS Name
{
	$$ = (struct TableList *) malloc (sizeof (struct TableList));
	$$->tableName = $3;
	$$->aliasAs = $5;
	$$->next = $1;
};

Table: Name
{
	$$ = (struct TableList *) malloc (sizeof (struct TableList));
	$$->tableName = $1;
	$$->aliasAs = NULL;
	$$->next = NULL;
};

CompoundExp: SimpleExp Op CompoundExp
{
	$$ = (struct FuncOperator *) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = (struct FuncOperator *) malloc (sizeof (struct FuncOperator));
	$$->leftOperator->leftOperator = NULL;
	$$->leftOperator->leftOperand = $1;
	$$->leftOperator->right = NULL;
	$$->leftOperand = NULL;
	$$->right = $3;
	$$->code = $2;	

}

| '(' CompoundExp ')' Op CompoundExp
{
	$$ = (struct FuncOperator *) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = $2;
	$$->leftOperand = NULL;
	$$->right = $5;
	$$->code = $4;	

}

| '(' CompoundExp ')'
{
	$$ = $2;

}

| SimpleExp
{
	$$ = (struct FuncOperator *) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = NULL;
	$$->leftOperand = $1;
	$$->right = NULL;	

}

| '-' CompoundExp
{
	$$ = (struct FuncOperator *) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = $2;
	$$->leftOperand = NULL;
	$$->right = NULL;	
	$$->code = '-';

}
;

Op: '-'
{
	$$ = '-';
}

| '+'
{
	$$ = '+';
}

| '*'
{
	$$ = '*';
}

| '/'
{
	$$ = '/';
}
;

AndList: '(' OrList ')' AND AndList
{
        // here we need to pre-pend the OrList to the AndList
        // first we allocate space for this node
        $$ = (struct AndList *) malloc (sizeof (struct AndList));

        // hang the OrList off of the left
        $$->left = $2;

        // hang the AndList off of the right
        $$->rightAnd = $5;

}

| '(' OrList ')'
{
        // just return the OrList!
        $$ = (struct AndList *) malloc (sizeof (struct AndList));
        $$->left = $2;
        $$->rightAnd = NULL;
}
;

OrList: Condition OR OrList
{
        // here we have to hang the condition off the left of the OrList
        $$ = (struct OrList *) malloc (sizeof (struct OrList));
        $$->left = $1;
        $$->rightOr = $3;
}

| Condition
{
        // nothing to hang off of the right
        $$ = (struct OrList *) malloc (sizeof (struct OrList));
        $$->left = $1;
        $$->rightOr = NULL;
}
;

Condition: Literal BoolComp Literal
{
        // in this case we have a simple literal/variable comparison
        $$ = $2;
        $$->left = $1;
        $$->right = $3;
}
;

BoolComp: '<'
{
        // construct and send up the comparison
        $$ = (struct ComparisonOp *) malloc (sizeof (struct ComparisonOp));
        $$->code = LESS_THAN;
}

| '>'
{
        // construct and send up the comparison
        $$ = (struct ComparisonOp *) malloc (sizeof (struct ComparisonOp));
        $$->code = GREATER_THAN;
}

| '='
{
        // construct and send up the comparison
        $$ = (struct ComparisonOp *) malloc (sizeof (struct ComparisonOp));
        $$->code = EQUALS;
}
;

Literal : String
{
        // construct and send up the operand containing the string
        $$ = (struct Operand *) malloc (sizeof (struct Operand));
        $$->code = STRING;
        $$->value = $1;
}

| Float
{
        // construct and send up the operand containing the FP number
        $$ = (struct Operand *) malloc (sizeof (struct Operand));
        $$->code = DOUBLE;
        $$->value = $1;
}

| Int
{
        // construct and send up the operand containing the integer
        $$ = (struct Operand *) malloc (sizeof (struct Operand));
        $$->code = INT;
        $$->value = $1;
}

| Name
{
        // construct and send up the operand containing the name
        $$ = (struct Operand *) malloc (sizeof (struct Operand));
        $$->code = NAME;
        $$->value = $1;
}
;


SimpleExp: 

Float
{
        // construct and send up the operand containing the FP number
        $$ = (struct FuncOperand *) malloc (sizeof (struct FuncOperand));
        $$->code = DOUBLE;
        $$->value = $1;
} 

| Int
{
        // construct and send up the operand containing the integer
        $$ = (struct FuncOperand *) malloc (sizeof (struct FuncOperand));
        $$->code = INT;
        $$->value = $1;
} 

| Name
{
        // construct and send up the operand containing the name
        $$ = (struct FuncOperand *) malloc (sizeof (struct FuncOperand));
        $$->code = NAME;
        $$->value = $1;
}
;

%%

