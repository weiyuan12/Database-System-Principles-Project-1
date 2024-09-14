#include <iostream>
#include <cassert>
#include "BPTreeNode.cpp"
#include "constants.h"

void test_BPTreeNode_constructor_and_serialize()
{
    // Create a buffer to simulate a block of memory
    char buffer[BLOCK_SIZE];

    // Create a BPTreeNode object
    int n = 3;
    int keys[MAX_INDEX_PER_BLOCK] = {3, 6, 9};
    int childrenPtr[MAX_INDEX_PER_BLOCK + 1] = {4, 5, 8, 11};

    BPTreeNode node(n, keys, childrenPtr);

    // Serialize the BPTreeNode object
    node.serialize(buffer);

    // Create a new BPTreeNode object from the serialized data
    BPTreeNode newNode(buffer);

    // Check if the new BPTreeNode object has the same values as the original object
    assert(newNode.numKeys == n);

    for (int i = 0; i < MAX_INDEX_PER_BLOCK; i++)
    {
        assert(newNode.keys[i] == keys[i]);
        assert(newNode.childrenPtr[i] == childrenPtr[i]);
    }

    assert(newNode.childrenPtr[MAX_INDEX_PER_BLOCK] == childrenPtr[MAX_INDEX_PER_BLOCK]);

    std::cout << "BPTreeNode constructor and serialize test passed!" << std::endl;
}

int main()
{
    test_BPTreeNode_constructor_and_serialize();
    return 0;
}