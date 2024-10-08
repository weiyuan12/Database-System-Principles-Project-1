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
#include <unordered_map>

class BPTree
{
private:
    void readMetadata(void *buffer);

public:
    BPTreeMetadata *metadata;
    BPTreeNode *root; // Pointer to the root node of the B-tree
    Storage *storage;

    int find(int key);
    void printIndexBlock(int blockNumber);
    void findRange(float startKey, float endKey, std::vector<int> *result);
    void bulkWriteToStorage(std::vector<BPTreeNode> &allBPTreeNodes, int depth, int rootIndex);

    BPTree(Storage *storage);
    BPTree();
    ~BPTree();
};

// TODO: Implement the find method
// We can treat -1 as cannot find the key
int BPTree::find(int key)
{
    BPTreeNode *currentNode = root;
    int currentDepth = 0;

    while (currentDepth <= metadata->depth)
    {
        int i = 0;
        if (currentDepth == metadata->depth)
        {
            // Find the appropriate child pointer
            while (i < currentNode->indexBlock->count && key > currentNode->indexBlock->keys[i])
            {
                i++;
            }
            return currentNode->indexBlock->childrenPtr[i];
        }

        // Find the appropriate child pointer
        while (i < currentNode->indexBlock->count && key >= currentNode->indexBlock->keys[i])
        {
            i++;
        }

        // Read the next node from storage
        char *buffer = new char[BLOCK_SIZE];
        storage->readBlock(buffer, currentNode->indexBlock->childrenPtr[i] + 1);
        BPTreeNode *nextNode = new BPTreeNode(buffer);
        currentNode = nextNode;
        currentDepth++;
    }
    return -1; // Return -1 if the key is not found
}

void BPTree::printIndexBlock(int blockNumber)
{
    char *buffer = new char[BLOCK_SIZE];
    storage->readBlock(buffer, blockNumber);
    IndexBlock *indexBlock = reinterpret_cast<IndexBlock *>(buffer);
    std::cout << "Block Number: " << blockNumber << std::endl;
    std::cout << "Count/Entries: " << indexBlock->count << std::endl;
    std::cout << "Keys: ";
    for (int i = 0; i < indexBlock->count; i++)
    {
        std::cout << indexBlock->keys[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Children: ";
    for (int i = 0; i < indexBlock->count + 1; i++)
    {
        std::cout << indexBlock->childrenPtr[i] + 1 << " ";
    }
    std::cout << std::endl;
}

void BPTree::findRange(float startKey, float endKey, std::vector<int> *result)
{
    BPTreeNode *currentNode = root; // Start from the root node
    // printIndexBlock(metadata->rootIndex);
    int currentDepth = 0; // Initialize the current depth to 0
    // int lastBlock = storage->getNumberOfBlocks() - 1;

    int startKeyInt = static_cast<int>(round(startKey * 1000));
    int endKeyInt = static_cast<int>(round(endKey * 1000));

    // Traverse the B+ tree until the leaf level
    while (currentDepth <= metadata->depth)
    {
        int i = 0;
        if (currentDepth == metadata->depth)
        {
            // Find the appropriate child pointer
            while (i < currentNode->indexBlock->count && startKeyInt > currentNode->indexBlock->keys[i])
            {
                i++;
            }

            // Traverse the leaf nodes to find the keys in the range
            while (currentNode->indexBlock->keys[i] <= endKeyInt && currentNode->indexBlock->keys[i] != 0)
            {
                result->push_back(currentNode->indexBlock->childrenPtr[i]);
                i++;
                if (i == currentNode->indexBlock->count)
                {
                    char *buffer = new char[BLOCK_SIZE];
                    if (currentNode->indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] == 0)
                    {
                        return;
                    }
                    storage->readBlock(buffer, currentNode->indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] + 1); // +1 to skip the metadata block
                    BPTreeNode *nextNode = new BPTreeNode(buffer);

                    currentNode = nextNode;
                    i = 0;
                }
            }
            return;
        }

        // Find the appropriate child pointer
        while (i < currentNode->indexBlock->count && startKeyInt > currentNode->indexBlock->keys[i])
        {
            i++;
        }
        // if (i > 0 && startKeyInt < currentNode->indexBlock->keys[i])
        // {
        //     i--;
        // }

        // Read the next node from storage
        char *buffer = new char[BLOCK_SIZE];
        storage->readBlock(buffer, currentNode->indexBlock->childrenPtr[i] + 1); // +1 to skip the metadata block
        // printIndexBlock(currentNode->indexBlock->childrenPtr[i] + 1);
        BPTreeNode *nextNode = new BPTreeNode(buffer);
        currentNode = nextNode; // Move to the next node
        currentDepth++;         // Increment the depth
    }
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
    // Dead
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
    // this is not the way to use BPTree
}

BPTree::BPTree(Storage *storage)
{
    this->storage = storage;
    char *metadataBuffer = new char[BLOCK_SIZE];
    storage->readBlock(metadataBuffer, 0);
    this->metadata = reinterpret_cast<BPTreeMetadata *>(metadataBuffer);

    // Read the root node
    this->root = new BPTreeNode();
    this->root->indexBlock = new IndexBlock();
    char *rootBuffer = new char[BLOCK_SIZE];
    storage->readBlock(rootBuffer, metadata->rootIndex);
    this->root->indexBlock = reinterpret_cast<IndexBlock *>(rootBuffer);
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
            bpTreeNode.indexBlock->keys[j] = childrenPtrs[index + 1].minKey;
            bpTreeNode.indexBlock->childrenPtr[j] = index + offset;
            bpTreeNode.indexBlock->count++;
            index++;
        }
        // last block and not full
        if (i + 1 == numIndexBlocks && numChildren % (MAX_INDEX_PER_BLOCK + 1) != 0)
        {
            int lastChildPtrIndex = numChildren % (MAX_INDEX_PER_BLOCK + 1) - 1;
            bpTreeNode.indexBlock->childrenPtr[lastChildPtrIndex] = index + offset;
        }
        else
        {
            bpTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = index + offset;
        }
        index++;

        parentBlocks.emplace_back(bpTreeNode);
    }
    offset += numIndexBlocks;
}

void balanceLastNode(std::vector<BPTreeNode> &leafBPTreeNodes)
{

    BPTreeNode *lastNode = &leafBPTreeNodes.back();
    BPTreeNode *secondLastNode = &leafBPTreeNodes[leafBPTreeNodes.size() - 2];

    int lastNodeSize = lastNode->indexBlock->count;
    if (lastNodeSize < (MAX_INDEX_PER_BLOCK + 1) / 2 && leafBPTreeNodes.size() > 1)
    {
        int keysToMove = (MAX_INDEX_PER_BLOCK + 1) / 2 - lastNodeSize;

        std::copy(lastNode->indexBlock->keys, lastNode->indexBlock->keys + lastNode->indexBlock->count, lastNode->indexBlock->keys + keysToMove);
        std::copy(lastNode->indexBlock->childrenPtr, lastNode->indexBlock->childrenPtr + lastNode->indexBlock->count, lastNode->indexBlock->childrenPtr + keysToMove);

        std::copy(secondLastNode->indexBlock->keys + (secondLastNode->indexBlock->count - keysToMove), secondLastNode->indexBlock->keys + (secondLastNode->indexBlock->count), lastNode->indexBlock->keys);
        std::copy(secondLastNode->indexBlock->childrenPtr + (secondLastNode->indexBlock->count - keysToMove), secondLastNode->indexBlock->childrenPtr + (secondLastNode->indexBlock->count), lastNode->indexBlock->childrenPtr);

        std::fill(secondLastNode->indexBlock->keys + (secondLastNode->indexBlock->count - keysToMove), secondLastNode->indexBlock->keys + (secondLastNode->indexBlock->count), 0);
        std::fill(secondLastNode->indexBlock->childrenPtr + (secondLastNode->indexBlock->count - keysToMove), secondLastNode->indexBlock->childrenPtr + (secondLastNode->indexBlock->count), 0);

        lastNode->indexBlock->count = lastNode->indexBlock->count + keysToMove;
        secondLastNode->indexBlock->count = secondLastNode->indexBlock->count - keysToMove;

        lastNode->minKey = lastNode->indexBlock->keys[0];
    }
}

void buildLeafLevel(std::vector<int> &gameEntryBlocks, std::vector<BPTreeNode> &leafBPTreeNodes)
{
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
        bPTreeNode.minKey = gameEntryBlocks[i * MAX_INDEX_PER_BLOCK];

        if (i + 1 < numIndexBlocks)
        {
            bPTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = i + 1;
        }
        else
        {
            bPTreeNode.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = 0;
        }
        leafBPTreeNodes.push_back(bPTreeNode);
    }
}

void buildBPTree(std::vector<GameEntryBlock> &gameEntryBlocks, std::vector<BPTreeNode> &allBPTreeNodes, int *depth, int *rootIndex)
{
    // Assert that gameEntryBlocks is sorted
    assert(std::is_sorted(gameEntryBlocks.begin(), gameEntryBlocks.end(), [](const GameEntryBlock &a, const GameEntryBlock &b)
                          { return a.entries[0].FG_PCT_home < b.entries[0].FG_PCT_home; }));

    allBPTreeNodes = std::vector<BPTreeNode>();
    std::vector<BPTreeNode> leafBPTreeNodes = std::vector<BPTreeNode>();
    std::vector<int> allChildrenKeys = std::vector<int>();

    // build children pointers
    for (int i = 0; i < gameEntryBlocks.size(); i++)
    {
        for (int j = 0; j < gameEntryBlocks[i].count; j++)
        {
            // check if nan
            if (std::isnan(gameEntryBlocks[i].entries[j].FG_PCT_home))
            {
                allChildrenKeys.push_back(-1);
                continue;
            }

            allChildrenKeys.push_back(static_cast<int>(round(gameEntryBlocks[i].entries[j].FG_PCT_home * 1000)));
        }
    }

    buildLeafLevel(allChildrenKeys, leafBPTreeNodes);
    balanceLastNode(leafBPTreeNodes);

    for (int i = 0; i < leafBPTreeNodes.size(); i++)
    {
        allBPTreeNodes.push_back(leafBPTreeNodes[i]);
    }

    int offset = 0;
    std::vector<BPTreeNode> currentChildren = leafBPTreeNodes;
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