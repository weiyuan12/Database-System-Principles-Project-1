#include <iostream>
#include <stdio.h>
#include <fstream>
#include "BPTree.cpp"
// #include "Storage.cpp"
#include <cassert>

void testBPTreeInitialization()
{
    // Create a temporary file for testing
    std::fstream file("testBPTreeInitialization", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Failed to create temporary file" << std::endl;
        return;
    }

    // Create a storage object with the temporary file
    Storage storage = Storage(&file);

    // Writing to metadata block
    char *blockData = new char[BLOCK_SIZE];
    // Write number 5 to the first 4 bytes of buffer
    int number = 5;
    memcpy(blockData, &number, sizeof(int));
    storage.addBlock(blockData);

    // Writing to root node block
    int cnt = 3;
    int keys[3] = {1, 2, 3};
    int children[4] = {0, 1, 2, 3};
    BPTreeNode rootNode = BPTreeNode(cnt, keys, children);

    blockData = new char[BLOCK_SIZE];
    rootNode.serialize(blockData);
    storage.addBlock(blockData);

    BPTree bptree = BPTree(&storage);

    assert(bptree.depth == 5);
    assert(bptree.root->numKeys == 3);
    assert(bptree.root->keys[0] == 1);
    assert(bptree.root->keys[1] == 2);
    assert(bptree.root->keys[2] == 3);
    assert(bptree.root->childrenPtr[0] == 0);
    assert(bptree.root->childrenPtr[1] == 1);
    assert(bptree.root->childrenPtr[2] == 2);
    assert(bptree.root->childrenPtr[3] == 3);

    std::cout << "B+ tree initialization test passed!" << std::endl;

    // Clean up
    delete[] blockData;
    file.close();
    std::remove("testBPTreeInitialization");
}

int main()
{
    testBPTreeInitialization();
    return 0;
}