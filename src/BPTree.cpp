

/**
 * @file btree_metadata.h
 * @brief This file contains the metadata for the B-tree and handles file operations.
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "BPTreeNode.cpp"
#include "Storage.cpp"

class BPTree
{
private:
    void readMetadata(void *buffer);

public:
    int depth;        // Depth of the B-tree
    BPTreeNode *root; // Pointer to the root node of the B-tree
    Storage *storage;

    BPTree(FILE *file);
    ~BPTree();
};

void BPTree::readMetadata(void *buffer)
{
    // Cast the buffer pointer to the appropriate type
    char *block = static_cast<char *>(buffer);

    // Read the depth of the B-tree from the block
    depth = *reinterpret_cast<int *>(block);

    // Read the root node from the block
    root = new BPTreeNode(storage->readBlock(1));
}

BPTree::BPTree(FILE *file)
{
    this->storage = new Storage(file);
    void *buffer = storage->readBlock(0);
    readMetadata(buffer);
    root = new BPTreeNode(storage->readBlock(1));
}

BPTree::~BPTree()
{
}
