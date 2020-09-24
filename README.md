## Database System Implementation
It is an assignment of course Database System Implementation (COP6726) taught in University of Florida.

## About the Project
* Designed a single-user database management system that supports a subset of SQL. 
* Implemented heap and sorted file organization to manage database records. 
* Added some basic relational algebra operations like select, project, join, sum, group by, and duplicate removal. 
* Devised query optimization module to decide and execute the optimized query plan using histograms.
* Project was developed in 7 different milestones.
* Assignment requirement documents can be found [here](https://github.com/dhiraj-paryani/database-system-implementation/tree/master/assignments/docs).
* Assignment reports can be found [here](https://github.com/dhiraj-paryani/database-system-implementation/tree/master/assignments/reports).
* GTests can be found [here](https://github.com/dhiraj-paryani/database-system-implementation/tree/master/gtests).

## Test Different milestones

### Initial Setup
  * To test the database, we are using [tpch-dbgen](https://github.com/electrum/tpch-dbgen.git) as sample data.
  * Clone the [tpch-dbgen](https://github.com/electrum/tpch-dbgen.git) repo to test.
  * Compile it:`make`
  * To generate 10MB data, run `./dbgen -s 0.01`. To generated 1GB data `./dbgen -s 1`.
  * This will generate 8 *.tbl files containing the data in CSV format with | separator

### Milestone - 1: Heap File Implementation
#### Description:
In this records are stored in an unordered fashion, where new records simply go at the end of the file. We can load records from a file, insert single record and fetch records simply and on filter basis.
#### Instructions to run tests:
* `make clean; make a1test.out; ./a1test.out`
* This would give a menu-based interface as following: 
  1. load (loads a heap DBFile from tpch-dbgen .tbl file)
  2. scan (read records from an existing heap DBFile)
  3. scan & filter (read records from an existing heap DBFile and filter using a CNF predicate)
* Follow the instructions to test the heap file implementation.

### Milestone - 2.1: BigQ Implementation
### Milestone - 2.2: Sorted File Implementation
### Milestone - 3: Relational Operations
### Milestone - 4.1: Statistical Estimation
### Milestone - 4.2: Query Compilation and Optimization
### Milestone - 5: Putting it All Together
