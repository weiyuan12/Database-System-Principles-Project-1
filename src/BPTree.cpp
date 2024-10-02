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
    int depth;        // Depth of the B-tree
    BPTreeNode *root; // Pointer to the root node of the B-tree
    Storage *storage;

    int find(int key);
    void bulkLoad(/*args*/);
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
    //
    //

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

void balanceLastNode(std::vector<BPTreeNode> &leafPBTreeNodes)
{
    BPTreeNode &lastNode = leafPBTreeNodes.back();
    BPTreeNode &secondLastNode = leafPBTreeNodes[leafPBTreeNodes.size() - 2];

    int lastNodeSize = lastNode.indexBlock->count;
    if(lastNodeSize < (MAX_INDEX_PER_BLOCK + 1) / 2 && leafPBTreeNodes.size() > 1)
    {
        int keysToMove = (MAX_INDEX_PER_BLOCK + 1) / 2 - lastNodeSize;
        
        std::copy( lastNode.indexBlock->keys,  lastNode.indexBlock->keys + lastNode.indexBlock->count, lastNode.indexBlock->keys + keysToMove );
        std::copy( lastNode.indexBlock->childrenPtr,  lastNode.indexBlock->childrenPtr + lastNode.indexBlock->count, lastNode.indexBlock->childrenPtr + keysToMove);

        std::copy(secondLastNode.indexBlock->keys + (secondLastNode.indexBlock->count - keysToMove), secondLastNode.indexBlock->keys + (secondLastNode.indexBlock->count),  lastNode.indexBlock->keys);
        std::copy(secondLastNode.indexBlock->childrenPtr + (secondLastNode.indexBlock->count - keysToMove), secondLastNode.indexBlock->childrenPtr + (secondLastNode.indexBlock->count),  lastNode.indexBlock->childrenPtr);
        
        std::fill(secondLastNode.indexBlock->keys + (secondLastNode.indexBlock->count - keysToMove), secondLastNode.indexBlock->keys + (secondLastNode.indexBlock->count), 0);
        std::fill(secondLastNode.indexBlock->childrenPtr + (secondLastNode.indexBlock->count - keysToMove), secondLastNode.indexBlock->childrenPtr + (secondLastNode.indexBlock->count), 0);

        lastNode.indexBlock->count = lastNode.indexBlock->count + keysToMove;
        secondLastNode.indexBlock->count = secondLastNode.indexBlock->count - keysToMove;
    }

}

void buildLeafLevel(std::vector<int> &gameEntryBlocks, std::vector<BPTreeNode> &leafPBTreeNodes)
{
    // I think leaf nods no need offset
    int numChildren = gameEntryBlocks.size();
    int numIndexBlocks = ceil((double)numChildren / MAX_INDEX_PER_BLOCK);
    int index = 0;

    // Creating leaf blocks
    for (int i = 0; i < numIndexBlocks; i++)
    {
        BPTreeNode bPTreeNode;
        bPTreeNode.indexBlock->count = 0;

        // Creating keys and pointers in each leaf node
        for (int j = 0; j < MAX_INDEX_PER_BLOCK && index < numChildren; j++)
        {
            bPTreeNode.indexBlock->keys[j] = gameEntryBlocks[index];
            bPTreeNode.indexBlock->childrenPtr[j] = i * MAX_INDEX_PER_BLOCK + j;
            bPTreeNode.indexBlock->count++;
            index++;
        }

        // Maybe for indexing
        bPTreeNode.minKey = gameEntryBlocks[i * MAX_INDEX_PER_BLOCK]; // ?

        // Last pointer points to next leafnode
        if (i + 1 < numIndexBlocks)
        {
            bPTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = i + 1;
        }
        else
        {
            bPTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = -1;
        }
        leafPBTreeNodes.push_back(bPTreeNode);

        std::cout << "indexBlock count: " << leafPBTreeNodes.back().indexBlock->count << std::endl;
        // if the last node has less than (n+1)/2 keys, rebalance keys from the secondLastNode
        balanceLastNode(leafPBTreeNodes);
    }
}

void buildBPTree(std::vector<GameEntryBlock> &gameEntryBlocks, std::vector<BPTreeNode> &allBPTreeNodes)
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
            allChildrenKeys.push_back(gameEntryBlocks[i].entries[j].FG_PCT_home);
        }
    }

    buildLeafLevel(allChildrenKeys, leafPBTreeNodes);

    for (int i = 0; i < leafPBTreeNodes.size(); i++)
    {
        allBPTreeNodes.push_back(leafPBTreeNodes[i]);
    }

    int offset = 0;
    int depth = 0;
    std::vector<BPTreeNode> currentChildren = leafPBTreeNodes;
    while (currentChildren.size() > 1)
    {
        depth++;
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
}
