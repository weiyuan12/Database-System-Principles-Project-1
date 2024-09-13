
#include <iostream>
#include "constants.h"

#include <cmath>
#include <cstring> // Add this line to include the <cstring> header ?

class bTreeNode
{
private:
public:
    // These values need to be stored in the block
    int n;                                    // Number of entries in the node
    int keys[MAX_INDEX_PER_BLOCK];            // Array of keys
    int childrenPtr[MAX_INDEX_PER_BLOCK + 1]; // Array of child keys

    // These values do not need to be stored in the block
    int depth; // Depth of the node in the tree

    bTreeNode(void *buffer);

    bTreeNode(int n, int keys[], int childrenPtr[]);
    void serialize(void *buffer);
};

bTreeNode::bTreeNode(void *buffer)
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

bTreeNode::bTreeNode(int n, int keys[], int childrenPtr[])
{
    this->n = n;
    memcpy(this->keys, keys, MAX_INDEX_PER_BLOCK * sizeof(int));
    memcpy(this->childrenPtr, childrenPtr, (MAX_INDEX_PER_BLOCK + 1) * sizeof(int));
}

void bTreeNode::serialize(void *buffer)
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
