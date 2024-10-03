# Database-System-Principles-Project-1
## TODO:

- [ ] Write a README.md file for how to run the code
- [x] Fix storage to use an internal cache to reduce disk I/O (like query with 0.450 instead of 450)
- [x] Fix the query key used in the query function
- [ ] Varify the correctness of the code
- - [ ] The results is correct
- - [ ] The Number of I/Os is correct for index and data blocks


- [x] Bug where if the block size is not 64, it will not work


## Prerequisites

Ensure you have the following installed on your system:

- A C++ compiler (e.g., g++, clang++).
- CMake (optional, for building the project).
- Git (optional, for cloning the repository).

## Installation

### Cloning the Repository

Clone this repository to your local machine:

```bash
git clone https://github.com/weiyuan12/Database-System-Principles-Project-1.git
```

### Building the Project

1. Open a terminal.
2. Navigate to the src directory.
3. Compile the code:

Windows
```bash
g++ -o Entry.exe main.cpp BPTree.cpp BPTreeNode.cpp DataFile.cpp DataFileReader.cpp Storage.cpp
```
Linux and macOS
```bash
g++ -o Entry main.cpp BPTree.cpp BPTreeNode.cpp DataFile.cpp DataFileReader.cpp Storage.cpp
```

### Usage
To run the program, execute the following command:

Windows
```bash
./Entry.exe
```
Linux and macOS
```bash
./Entry
```

### Testing
To run tests for the B-Tree implementation, you can compile and execute the test files.

Windows
```bash
g++ -o test_BPTree test_BPTree.cpp BPTree.cpp BPTreeNode.cpp
./test_BPTree
```
Linux and macOS
```bash
g++ -o test_BPTree test_BPTree.cpp BPTree.cpp BPTreeNode.cpp
./test_BPTree
```
