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

    int find(int key);
    void bulkLoad(/*args*/);
    BPTree(Storage *storage);
    ~BPTree();
};

// TODO: Implement the find method
// We can treat -1 as cannot find the key
int BPTree::find(int key)
{
    return 0;
}

// TODO: Implement Bulk Load
void BPTree::bulkLoad(/*args*/)
{
    // Implement bulk load here
}

void BPTree::readMetadata(void *buffer)
{
    // Cast the buffer pointer to the appropriate type
    char *block = static_cast<char *>(buffer);

    // Read the depth of the B-tree from the block
    depth = *reinterpret_cast<int *>(block);

    // Read the root node from the block
    char *rootBuffer = new char[BLOCK_SIZE];
    storage->readBlock(rootBuffer, 1); // first block is metadata
    root = new BPTreeNode(rootBuffer);
}

BPTree::BPTree(Storage *storage)
{
    this->storage = storage;
    char *metadataBuffer = new char[BLOCK_SIZE];
    storage->readBlock(metadataBuffer, 0);
    readMetadata(metadataBuffer);
}

BPTree::~BPTree()
{
    delete root;
}