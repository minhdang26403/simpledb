# SimpleDB

[SimpleDB](http://www.cs.bc.edu/~sciore/simpledb/) is a multi-user transactional database server written in Java. Its architecture is explained in detail in the book [Database Design and Implementation](https://link.springer.com/book/10.1007/978-3-030-33836-7). This project aims to implement SimpleDB using C++20.

## Implementation Plan

- [x] File Manager
- [x] Log Manager
- [x] Buffer Manager
- [x] Recovery Manager
- [x] Concurrency Manager
- [x] Transaction
- Record Manager
- Metadata Manager
- Select Scans, Project Scans, Product Scans
- Parser
- Planner
- Static Hash Indexes
- Btree Indexes
- Materialization and Sorting
- MultiBuffer Sorting/Product
- Query Optimization

## Optimization Plan
- Log Manager:
  + Log Manager and Log Iterator currently allocate their own memory page, change the implementation to use buffers from the buffer pool instead
- Buffer Manager:
  + Keep a mapping from each block to the buffer holding that block (instead of a sequential scan)
  + Use a more clever buffer replacement strategy
- Recovery Manager:
  + The recovery algorithm does not look at the current state of the database, making substantial number of unnecessary disk writes if the database is large

## Build

Run the following commands to build the system:
```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

If you want to compile the system in debug mode, pass in the following flag to CMake:
```
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
$ cmake --build .
```

If you want to compile the system in release mode, pass in the following flag to CMake:
```
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build .
```