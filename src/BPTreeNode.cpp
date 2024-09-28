
#include <iostream>
#include "constants.h"

#include <cmath>
#include <cstring> // Add this line to include the <cstring> header ?

// (BLCOK_SIZE - number_of_keys - childrenPtr) / (key + ptr)
class BPTreeNode
{
private:
public:
    IndexBlock *indexBlock;
    int minKey;

    BPTreeNode(void *buffer);
    BPTreeNode(int n, int keys[], int childrenPtr[]);
    BPTreeNode();
    void serialize(void *buffer);
};

BPTreeNode::BPTreeNode(void *buffer)
{
    // Create a buffer to simulate a block of memory
    char *block = static_cast<char *>(buffer);

    // Create a BPTreeNode object
    indexBlock = new IndexBlock();

    // Read the number of entries from the block
    indexBlock->count = *reinterpret_cast<int *>(block);

    // Read the keys from the block
    memcpy(indexBlock->keys, block + sizeof(int), MAX_INDEX_PER_BLOCK * sizeof(int));

    // Read the child keys from the block
    memcpy(indexBlock->childrenPtr, block + sizeof(int) + MAX_INDEX_PER_BLOCK * sizeof(int), (MAX_INDEX_PER_BLOCK + 1) * sizeof(int));
}

BPTreeNode::BPTreeNode(int n, int keys[], int childrenPtr[])
{
    indexBlock = new IndexBlock();
    indexBlock->count = n;
    memcpy(indexBlock->keys, keys, MAX_INDEX_PER_BLOCK * sizeof(int));
    memcpy(indexBlock->childrenPtr, childrenPtr, (MAX_INDEX_PER_BLOCK + 1) * sizeof(int));
}

BPTreeNode::BPTreeNode()
{
    indexBlock = new IndexBlock();
    indexBlock->count = 0;
}

void BPTreeNode::serialize(void *buffer)
{
    // Cast the buffer pointer to the appropriate type
    char *block = static_cast<char *>(buffer);

    // Write the number of entries to the block
    *reinterpret_cast<int *>(block) = indexBlock->count;

    // Write the keys to the block
    memcpy(block + sizeof(int), indexBlock->keys, MAX_INDEX_PER_BLOCK * sizeof(int));

    // Write the child keys to the block
    memcpy(block + sizeof(int) + MAX_INDEX_PER_BLOCK * sizeof(int), indexBlock->childrenPtr, (MAX_INDEX_PER_BLOCK + 1) * sizeof(int));
}
