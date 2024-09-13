#include <iostream>
#include <cassert>
#include "bTreeNode.cpp"
#include "constants.h"

void test_bTreeNode_constructor_and_serialize()
{
    // Create a buffer to simulate a block of memory
    char buffer[BLOCK_SIZE];

    // Create a bTreeNode object
    int n = 3;
    int keys[MAX_INDEX_PER_BLOCK] = {3, 6, 9};
    int childrenPtr[MAX_INDEX_PER_BLOCK + 1] = {4, 5, 8, 11};

    bTreeNode node(n, keys, childrenPtr);

    // Serialize the bTreeNode object
    node.serialize(buffer);

    // Create a new bTreeNode object from the serialized data
    bTreeNode newNode(buffer);

    // Check if the new bTreeNode object has the same values as the original object
    assert(newNode.n == n);

    for (int i = 0; i < MAX_INDEX_PER_BLOCK; i++)
    {
        assert(newNode.keys[i] == keys[i]);
        assert(newNode.childrenPtr[i] == childrenPtr[i]);
    }

    assert(newNode.childrenPtr[MAX_INDEX_PER_BLOCK] == childrenPtr[MAX_INDEX_PER_BLOCK]);

    std::cout << "bTreeNode constructor and serialize test passed!" << std::endl;
}

int main()
{
    test_bTreeNode_constructor_and_serialize();
    return 0;
}