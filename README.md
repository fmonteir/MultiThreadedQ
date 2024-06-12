# MultiThreadedQ

This repository contains a C++ implementation of a multi-threaded element queue. 

Unit tests are implemented with the framework Boost.Test (in header-only mode for convenience). The project build with CMake is described below.

For convenience, version 1.85.0 of the boost library can be found in the **include** directory, so there is no need to install it on your system in order to run the unit tests.

## Build process with CMake

To run the unit tests, the first step is to create and go into the build directory using 


mkdir build 


cd build


Then, run the command 


cmake ..


followed by 


cmake --build .


An executable is generated. In order to see log messages, run it using the command 


./unit_tests --log_level=message

## Unit tests

1. The first example is a queue with a maximum number of elements of 2. There are two threads: a writing thread and a reading thread. The first one pushes elements and the second one pops them. The following sequence is implemented in the example: Push(1), Pop(1), Push(2), Push(3), Push(4) -> triggers the dropping of element 2, Pop(3), Pop(4), Pop() -> blocks, Push(5), Element 5 is released.

2. The second example serves to verify that templates are working. I implement the same sequence of the first example, but instead of int (1, 2, 3, 4, 5), I now use char ('A', 'B', 'C', 'D', 'E').

3. The third example is a test with a longer queue with a maximum number of elements of 4. Once again, there are two threads: write and read. The sequence is now as follows: Push(1), Pop(1), Push(2), Push(3), Push(4), Push(5), Push(6) -> triggers the dropping of element 2, Pop(3), Pop(4), Pop(5), Pop(6) -> blocks, Push(7), Element 7 is released. 

