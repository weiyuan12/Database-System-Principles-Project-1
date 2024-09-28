/**
 * @file btree_metadata.h
 * @brief This file contains the metadata for the B-tree and handles file operations.
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include "BPTreeNode.cpp"
#include "Storage.cpp"
#include <algorithm>
#include <cassert>

class BPTree
{
private:
    void readMetadata(void *buffer);

public:
    BPTreeMetadata *metadata;
    BPTreeNode *root; // Pointer to the root node of the B-tree
    Storage *storage;

    int find(int key);
    void bulkLoad(/*args*/);
    void bulkWriteToStorage(std::vector<BPTreeNode> &allBPTreeNodes, int depth, int rootIndex);

    BPTree(Storage *storage);
    BPTree();
    ~BPTree();
};

// TODO: Implement the find method
// We can treat -1 as cannot find the key
int BPTree::find(int key)
{
    return 0;
}

// this is very buggy
void BPTree::bulkWriteToStorage(std::vector<BPTreeNode> &allBPTreeNodes, int depth, int rootIndex)
{
    // Write the metadata to the first block
    BPTreeMetadata metadata;
    metadata.depth = depth;
    metadata.rootIndex = rootIndex;
    char metadataBuffer[BLOCK_SIZE];

    memcpy(metadataBuffer, &metadata, sizeof(BPTreeMetadata));
    storage->addBlock(metadataBuffer);

    // Write the B-tree nodes to the subsequent blocks
    for (int i = 0; i < allBPTreeNodes.size(); i++)
    {
        char *nodeBuffer = new char[BLOCK_SIZE];
        allBPTreeNodes[i].serialize(nodeBuffer);
        storage->addBlock(nodeBuffer);
    }
}

void BPTree::readMetadata(void *buffer)
{
    metadata = new BPTreeMetadata();
    memcpy(metadata, buffer, sizeof(BPTreeMetadata));
    root = new BPTreeNode();
    root->indexBlock = new IndexBlock();
    char *rootBuffer = new char[BLOCK_SIZE];
    storage->readBlock(rootBuffer, metadata->rootIndex);
    root->serialize(rootBuffer);
}

BPTree::BPTree()
{
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

// === This are helper functions for building the B+ tree ===

void buildLevel(int offset, std::vector<BPTreeNode> &childrenPtrs, std::vector<BPTreeNode> &parentBlocks)
{
    // +---------------+ ~~ +----------------+--------------+--------------+
    // | leaf children | ~~ | 2 lvl children | childrenPtrs | parentBlocks |
    // +---------------+ ~~ +----------------+--------------+--------------+
    // |<------------- offset -------------->|

    int numChildren = childrenPtrs.size();
    int numIndexBlocks = ceil((double)numChildren / (MAX_INDEX_PER_BLOCK + 1));
    int index = 0;
    for (int i = 0; i < numIndexBlocks; i++)
    {
        BPTreeNode bpTreeNode = BPTreeNode();
        bpTreeNode.indexBlock->count = 0;
        bpTreeNode.minKey = childrenPtrs[index].minKey;
        for (int j = 0; j < MAX_INDEX_PER_BLOCK && index < numChildren - 1; j++)
        {
            // bpTreeNode.indexBlock->keys[j] = childrenPtrs[index].indexBlock->keys[0];
            bpTreeNode.indexBlock->keys[j] = childrenPtrs[index + 1].minKey;
            bpTreeNode.indexBlock->childrenPtr[j] = index + offset;
            bpTreeNode.indexBlock->count++;
            index++;
        }
        // bpTreeNode.minKey = childrenPtrs[index].indexBlock->keys[0];
        if (i + 1 == numIndexBlocks && numChildren % (MAX_INDEX_PER_BLOCK + 1) != 0) // last block and not full
        {
            int lastChildPtrIndex = numChildren % (MAX_INDEX_PER_BLOCK + 1) - 1;
            bpTreeNode.indexBlock->childrenPtr[lastChildPtrIndex] = index + offset;
        }
        else
        {
            bpTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = index + offset;
        }
        // bpTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = index + offset;
        index++;

        parentBlocks.emplace_back(bpTreeNode);
    }
    offset += numIndexBlocks;
}

void buildLeafLevel(std::vector<int> &gameEntryBlocks, std::vector<BPTreeNode> &leafPBTreeNodes)
{
    // I think leaf nods no need offset
    int numChildren = gameEntryBlocks.size();
    int numIndexBlocks = ceil((double)numChildren / MAX_INDEX_PER_BLOCK);
    int index = 0;
    for (int i = 0; i < numIndexBlocks; i++)
    {
        BPTreeNode bPTreeNode;
        bPTreeNode.indexBlock->count = 0;
        for (int j = 0; j < MAX_INDEX_PER_BLOCK && index < numChildren; j++)
        {
            bPTreeNode.indexBlock->keys[j] = gameEntryBlocks[index];
            bPTreeNode.indexBlock->childrenPtr[j] = i * MAX_INDEX_PER_BLOCK + j;
            bPTreeNode.indexBlock->count++;
            index++;
        }
        bPTreeNode.minKey = gameEntryBlocks[i * MAX_INDEX_PER_BLOCK]; // ?
        if (i + 1 < numIndexBlocks)
        {
            bPTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = i + 1;
        }
        else
        {
            bPTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = -1;
        }
        leafPBTreeNodes.push_back(bPTreeNode);
    }
}

void buildBPTree(std::vector<GameEntryBlock> &gameEntryBlocks, std::vector<BPTreeNode> &allBPTreeNodes, int *depth, int *rootIndex)
{
    // Assert that gameEntryBlocks is sorted
    assert(std::is_sorted(gameEntryBlocks.begin(), gameEntryBlocks.end(), [](const GameEntryBlock &a, const GameEntryBlock &b)
                          { return a.entries[0].FG_PCT_home < b.entries[0].FG_PCT_home; }));

    allBPTreeNodes = std::vector<BPTreeNode>();
    std::vector<BPTreeNode> leafPBTreeNodes = std::vector<BPTreeNode>();
    std::vector<int> allChildrenKeys = std::vector<int>();

    // build children pointers
    for (int i = 0; i < gameEntryBlocks.size(); i++)
    {
        for (int j = 0; j < gameEntryBlocks[i].count; j++)
        {
            allChildrenKeys.push_back(static_cast<int>(gameEntryBlocks[i].entries[j].FG_PCT_home * 1000));
        }
    }

    buildLeafLevel(allChildrenKeys, leafPBTreeNodes);

    for (int i = 0; i < leafPBTreeNodes.size(); i++)
    {
        allBPTreeNodes.push_back(leafPBTreeNodes[i]);
    }

    int offset = 0;
    std::vector<BPTreeNode> currentChildren = leafPBTreeNodes;
    while (currentChildren.size() > 1)
    {
        (*depth)++;
        std::vector<BPTreeNode> parentBlocks = std::vector<BPTreeNode>();
        int sizeCurrentChildren = currentChildren.size();
        buildLevel(offset, currentChildren, parentBlocks);
        offset += currentChildren.size();
        currentChildren = parentBlocks;
        for (int i = 0; i < currentChildren.size(); i++)
        {
            allBPTreeNodes.push_back(currentChildren[i]);
        }
    }

    *rootIndex = allBPTreeNodes.size();
}

void bptreeBlocksToStorage(std::vector<BPTreeNode> &allBPTreeNodes, int depth, int rootIndex, Storage *storage)
{
    // Write the metadata to the first block
    BPTreeMetadata metadata;
    metadata.depth = depth;
    metadata.rootIndex = rootIndex;
    char metadataBuffer[BLOCK_SIZE];

    memcpy(metadataBuffer, &metadata, sizeof(BPTreeMetadata));
    storage->addBlock(metadataBuffer);

    std::vector<IndexBlock> allIndexBlocks = std::vector<IndexBlock>();
    for (int i = 0; i < allBPTreeNodes.size(); i++)
    {
        allIndexBlocks.push_back(*allBPTreeNodes[i].indexBlock);
    }

    for (int i = 0; i < allIndexBlocks.size(); i++)
    {
        char *nodeBuffer = new char[BLOCK_SIZE];
        memcpy(nodeBuffer, &allIndexBlocks[i], sizeof(IndexBlock));
        storage->addBlock(nodeBuffer);
    }
}