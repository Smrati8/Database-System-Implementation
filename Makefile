
CC = g++ -O2 -Wno-deprecated

tag = -i

ifdef linux
tag = -n
endif

test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o Statistics.o QueryRunner.o QueryPlan.o RelOpPlanNode.o PathConfig.o Database.o test.o y.tab.latest.o lex.yy.latest.o
	$(CC) -o test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o Statistics.o QueryRunner.o QueryPlan.o RelOpPlanNode.o PathConfig.o Database.o test.o y.tab.o lex.yy.o -ll -lpthread

a42.out: y.tab.latest.o lex.yy.latest.o QueryPlan.o QueryPlanNode.o Statistics.o Record.o Comparison.o Schema.o Function.o a42-test.o
	$(CC) -o a42.out y.tab.o lex.yy.o QueryPlan.o QueryPlanNode.o Statistics.o Record.o Comparison.o Schema.o Function.o a42-test.o -ll

a4-1.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o Statistics.o y.tab.o lex.yy.o a41-test.o
	$(CC) -o a4-1.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o Statistics.o y.tab.o lex.yy.o a41-test.o -ll

a3test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o a3-test.o
	$(CC) -o a3test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o a3-test.o -ll -lpthread

a2test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o a2-test.o
	$(CC) -o a2test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o a2-test.o -ll -lpthread

a21test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o a21-test.o
	$(CC) -o a21test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o a21-test.o -ll -lpthread
	
a1test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o a1-test.o
	$(CC) -o a1test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o a1-test.o -ll -lpthread
	
gTestDBFile.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o DBFileGTests.o
	$(CC) -o gTestDBFile.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o DBFileGTests.o -ll -lpthread -lgtest

gTestBigQ.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o BigQGTests.o
	$(CC) -o gTestBigQ.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o BigQGTests.o -ll -lpthread -lgtest

gTestSortedDBFile.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o SortedDBFileGTests.o
	$(CC) -o gTestSortedDBFile.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o y.tab.o lex.yy.o SortedDBFileGTests.o -ll -lpthread -lgtest

gTestRelOp.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o RelOpGTests.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o
	$(CC) -o gTestRelOp.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o RelOpGTests.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o -ll -lpthread -lgtest

gTestStatistics.out: Statistics.o y.tab.o lex.yy.o StatisticsGTests.o
	$(CC) -o gTestStatistics.out Statistics.o y.tab.o lex.yy.o StatisticsGTests.o -ll -lgtest

gTestQueryPlan.out: QueryPlan.o QueryPlanNode.o Statistics.o Record.o Comparison.o Schema.o Function.o QueryPlanGTests.o
	$(CC) -o gTestQueryPlan.out QueryPlan.o QueryPlanNode.o Statistics.o Record.o Comparison.o Schema.o Function.o QueryPlanGTests.o -ll -lgtest

gTestDatabase.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o Statistics.o QueryRunner.o QueryPlan.o RelOpPlanNode.o PathConfig.o Database.o DatabaseGTests.o y.tab.latest.o lex.yy.latest.o
	$(CC) -o gTestDatabase.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Pipe.o BigQ.o RelOp.o Function.o Statistics.o QueryRunner.o QueryPlan.o RelOpPlanNode.o PathConfig.o Database.o DatabaseGTests.o y.tab.o lex.yy.o -ll -lpthread -lgtest

test.o: test.cc
	$(CC) -g -c test.cc

a42-test.o: tests/a42-test.cc
	$(CC) -g -c a42-test.cc

a41-test.o: tests/a41-test.cc
	$(CC) -g -c a41-test.cc

a3-test.o: tests/a3-test.cc
	$(CC) -g -c a3-test.cc

a2-test.o: tests/a2-test.cc
	$(CC) -g -c a2-test.cc

a21-test.o: tests/a21-test.cc
	$(CC) -g -c a21-test.cc

a1-test.o: tests/a1-test.cc
	$(CC) -g -c a1-test.cc

DBFileGTests.o: gtests/DBFileGTests.cc
	$(CC) -g -c gtests/DBFileGTests.cc

BigQGTests.o: gtests/BigQGTests.cc
	$(CC) -g -c gtests/BigQGTests.cc

SortedDBFileGTests.o: gtests/SortedDBFileGTests.cc
	$(CC) -g -c gtests/SortedDBFileGTests.cc

RelOpGTests.o: gtests/RelOpGTests.cc
	$(CC) -g -c gtests/RelOpGTests.cc

StatisticsGTests.o: gtests/StatisticsGTests.cc
	$(CC) -g -c gtests/StatisticsGTests.cc

QueryPlanGTests.o: gtests/QueryPlanGTests.cc
	$(CC) -g -c gtests/QueryPlanGTests.cc

DatabaseGTests.o: gtests/DatabaseGTests.cc
	$(CC) -g -c gtests/DatabaseGTests.cc

QueryRunner.o: QueryRunner.cc
	$(CC) -g -c QueryRunner.cc

QueryPlan.o: QueryPlan.cc
	$(CC) -g -c QueryPlan.cc

RelOpPlanNode.o: RelOpPlanNode.cc
	$(CC) -g -c RelOpPlanNode.cc

Database.o: Database.cc
	$(CC) -g -c Database.cc

PathConfig.o: PathConfig.cc
	$(CC) -g -c PathConfig.cc

Statistics.o: Statistics.cc
	$(CC) -g -c Statistics.cc

Comparison.o: Comparison.cc
	$(CC) -g -c Comparison.cc
	
ComparisonEngine.o: ComparisonEngine.cc
	$(CC) -g -c ComparisonEngine.cc
	
Pipe.o: Pipe.cc
	$(CC) -g -c Pipe.cc

BigQ.o: BigQ.cc
	$(CC) -g -c BigQ.cc

DBFile.o: DBFile.cc
	$(CC) -g -c DBFile.cc

GenericDBFile.o: GenericDBFile.cc
	$(CC) -g -c GenericDBFile.cc

HeapDBFile.o: HeapDBFile.cc
	$(CC) -g -c HeapDBFile.cc

SortedDBFile.o: SortedDBFile.cc
	$(CC) -g -c SortedDBFile.cc

RelOp.o: RelOp.cc
	$(CC) -g -c RelOp.cc

Function.o: Function.cc
	$(CC) -g -c Function.cc

File.o: File.cc
	$(CC) -g -c File.cc

Record.o: Record.cc
	$(CC) -g -c Record.cc

Schema.o: Schema.cc
	$(CC) -g -c Schema.cc

y.tab.latest.o: ParserLatest.y
	yacc -d ParserLatest.y
	sed $(tag) -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" y.tab.c
	g++ -c y.tab.c

y.tab.o: Parser.y
	yacc -d Parser.y
	#sed $(tag) y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	g++ -c y.tab.c
		
yyfunc.tab.o: ParserFunc.y
	yacc -p "yyfunc" -b "yyfunc" -d ParserFunc.y
	#sed $(tag) yyfunc.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	g++ -c yyfunc.tab.c

lex.yy.latest.o: LexerLatest.l
	lex  LexerLatest.l
	gcc  -c lex.yy.c

lex.yy.o: Lexer.l
	lex  Lexer.l
	gcc  -c lex.yy.c

lex.yyfunc.o: LexerFunc.l
	lex -Pyyfunc LexerFunc.l
	gcc  -c lex.yyfunc.c

clean: 
	rm -f *.o
	rm -f *.out
	rm -f y.tab.*
	rm -f yyfunc.tab.*
	rm -f lex.yy.*
	rm -f lex.yyfunc*
