
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
    // Cast the buffer pointer to the appropriate type
    char *block = static_cast<char *>(buffer);

    // Cast the buffer to an indexBlock
    indexBlock = reinterpret_cast<IndexBlock *>(block);
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

// this function might be useless
void BPTreeNode::serialize(void *buffer)
{
    // Cast the buffer pointer to the appropriate type
    char *block = static_cast<char *>(buffer);

    // Cast the indexBlock to a buffer
    memcpy(block, indexBlock, sizeof(IndexBlock));
}
