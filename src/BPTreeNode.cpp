
#include <iostream>
#include "constants.h"

#include <cmath>
#include <cstring> // Add this line to include the <cstring> header ?

// (BLCOK_SIZE - number_of_keys - childrenPtr) / (key + ptr)
const int MAX_INDEX_PER_BLOCK = (BLOCK_SIZE - sizeof(int) - sizeof(int)) / (sizeof(int) + sizeof(int));

class BPTreeNode
{
private:
public:
    // These values need to be stored in the block
    int n;                                    // Number of entries in the node
    int keys[MAX_INDEX_PER_BLOCK];            // Array of keys
    int childrenPtr[MAX_INDEX_PER_BLOCK + 1]; // Array of child keys
    // At the leaf level, the childrenPtr will be a pointer to the data block
    // Up to you to use sparse or dense index
    // - Sparse index: will allow more data to be stored
    // - Dense index: will search in O(1) instead of O(entries per block)

    // These values do not need to be stored in the block
    // We may not need depth as the depth of the tree can be calculated from the root node
    // All leaf nodes will have the same depth
    int depth; // Depth of the node in the tree

    BPTreeNode(void *buffer);

    BPTreeNode(int n, int keys[], int childrenPtr[]);
    void serialize(void *buffer);
};

BPTreeNode::BPTreeNode(void *buffer)
{
    // Cast the buffer pointer to the appropriate type
    char *block = static_cast<char *>(buffer);

    // Read the number of entries from the block
    n = *reinterpret_cast<int *>(block);

    // Read the keys from the block
    memcpy(keys, block + sizeof(int), MAX_INDEX_PER_BLOCK * sizeof(int));

    // Read the child keys from the block
    memcpy(childrenPtr, block + sizeof(int) + MAX_INDEX_PER_BLOCK * sizeof(int), (MAX_INDEX_PER_BLOCK + 1) * sizeof(int));
}

BPTreeNode::BPTreeNode(int n, int keys[], int childrenPtr[])
{
    this->n = n;
    memcpy(this->keys, keys, MAX_INDEX_PER_BLOCK * sizeof(int));
    memcpy(this->childrenPtr, childrenPtr, (MAX_INDEX_PER_BLOCK + 1) * sizeof(int));
}

void BPTreeNode::serialize(void *buffer)
{
    // Cast the buffer pointer to the appropriate type
    char *block = static_cast<char *>(buffer);

    // Write the number of entries to the block
    *reinterpret_cast<int *>(block) = n;

    // Write the keys to the block
    memcpy(block + sizeof(int), keys, MAX_INDEX_PER_BLOCK * sizeof(int));

    // Write the child keys to the block
    memcpy(block + sizeof(int) + MAX_INDEX_PER_BLOCK * sizeof(int), childrenPtr, (MAX_INDEX_PER_BLOCK + 1) * sizeof(int));
}
