# Database System Implementation
It is an project implementation of course Database System Implementation (COP6726) taught in University of Florida.

## About the Project
* Designed a single-user database management system that supports a subset of SQL. 
  - Implemented heap and sorted file organization to manage database records. 
  - Added some basic relational algebra operations like select, project, join, sum, group by, and duplicate removal. 
  - Devised query optimization module to decide and execute the optimized query plan using histograms.
  - Added lexer and parser to support sql statement given by user.
* Project was developed in 7 different milestones.
* Project documentation can be found [here](https://github.com/Smrati8/database-implementation/tree/master/documentation).
* GTests can be found [here](https://github.com/Smrati8/database-implementation/tree/master/gtests).

## Initial Setup
  * To test the database, we are using [tpch-dbgen](https://github.com/electrum/tpch-dbgen.git) as sample data.
  * Clone the [tpch-dbgen](https://github.com/electrum/tpch-dbgen.git) repo to test.
  * Compile it:`make`
  * To generate 10MB data, run `./dbgen -s 0.01`. To generated 1GB data `./dbgen -s 1`.
  * This will generate 8 *.tbl files containing the data in CSV format with | separator

## Different Milestones

| Milestone | Requirement and Implementation Details | Final Report and Sample Outputs | 
| --------- | -------------------------------------- | ------------------------------- |
| Heap File Implementation  | [Click here](https://github.com/Smrati8/database-implementation/blob/master/documentation/A1.pdf) | [Click here](https://github.com/Smrati8/database-implementation/blob/master/documentation/Report%20Assignment%201.pdf) | 
| BigQ Implementation  | [Click here] () | [Click here] () |
| Sorted File Implementation | [Click here] () | [Click here] () |
| Relational Operations | [Click here] () | [Click here] () |
| Statistical Estimation | [Click here] () | [Click here] () |
| Query Compilation and Optimization | [Click here] () | [Click here] () |
| Putting it All Together | [Click here] () | [Click here] () |
