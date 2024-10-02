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

    assert(bptree.metadata->depth == 5);
    assert(bptree.root->indexBlock->count == 3);
    assert(bptree.root->indexBlock->keys[0] == 1);
    assert(bptree.root->indexBlock->keys[1] == 2);
    assert(bptree.root->indexBlock->keys[2] == 3);
    assert(bptree.root->indexBlock->childrenPtr[0] == 0);
    assert(bptree.root->indexBlock->childrenPtr[1] == 1);
    assert(bptree.root->indexBlock->childrenPtr[2] == 2);
    assert(bptree.root->indexBlock->childrenPtr[3] == 3);

    std::cout << "B+ tree initialization test passed!" << std::endl;

    // Clean up
    delete[] blockData;
    file.close();
    std::remove("testBPTreeInitialization");
}

void testBulkWriteToStorage()
{
    // Create a temporary file for testing
    std::fstream file("testBulkWriteToStorage", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Failed to create temporary file" << std::endl;
        return;
    }

    // Create a storage object with the temporary file
    Storage storage = Storage(&file);

    bool verbose = true;
    std::vector<GameEntryBlock> gameEntryBlocks = std::vector<GameEntryBlock>();
    for (int i = 0; i < MAX_INDEX_PER_BLOCK * 100; i++)
    {
        GameEntryBlock block = GameEntryBlock();
        block.count = 0;
        for (int j = 0; j < MAX_ENTRIES_PER_BLOCK; j++)
        {
            GameEntry entry;
            entry.FG_PCT_home = (i * MAX_ENTRIES_PER_BLOCK + j) * 10;
            block.entries[j] = entry;
            block.count++;
        }
        gameEntryBlocks.push_back(block);
    }

    std::vector<BPTreeNode> allBPTreeNodes = std::vector<BPTreeNode>();
    int depth = 0;
    int rootIndex = 0;
    buildBPTree(gameEntryBlocks, allBPTreeNodes, &depth, &rootIndex);

    bptreeBlocksToStorage(allBPTreeNodes, depth, rootIndex, &storage);

    BPTree bptree = BPTree(&storage);

    std::cout << "B+ Tree depth: " << bptree.metadata->depth << std::endl;
    std::cout << "B+ Tree root index: " << bptree.metadata->rootIndex << std::endl;

    file.close();

    std::fstream file2("testBulkWriteToStorage", std::ios::binary | std::ios::in | std::ios::out);

    Storage storage2 = Storage(&file2);
    BPTree bptree2 = BPTree(&storage2);

    std::cout << "B+ Tree depth: " << bptree2.metadata->depth << std::endl;
    std::cout << "B+ Tree root index: " << bptree2.metadata->rootIndex << std::endl;

    int datablock = bptree2.find(30000);
    std::cout << "Data block: " << datablock << std::endl;
}

void testBPTree()
{
    testBPTreeInitialization();
}

void testBuildLeafLevelSmall()
{
    bool verbose = true;
    std::vector<int> gameEntryBlocks = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<BPTreeNode> leafPBTreeNodes = std::vector<BPTreeNode>();
    int offset = 0;
    buildLeafLevel(gameEntryBlocks, leafPBTreeNodes);

    if (verbose)
    {
        for (const auto &node : leafPBTreeNodes)
        {
            std::cout << "IndexBlock count: " << node.indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int i = 0; i < node.indexBlock->count; ++i)
            {
                std::cout << node.indexBlock->keys[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int i = 0; i <= node.indexBlock->count; ++i)
            {
                std::cout << node.indexBlock->childrenPtr[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << node.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
    }

    assert(leafPBTreeNodes.size() == 1);
    assert(leafPBTreeNodes[0].indexBlock->count == 10);
    assert(leafPBTreeNodes[0].indexBlock->keys[0] == 0);
    assert(leafPBTreeNodes[0].indexBlock->keys[9] == 9);
    assert(leafPBTreeNodes[0].indexBlock->childrenPtr[0] == 0);
    assert(leafPBTreeNodes[0].indexBlock->childrenPtr[9] == 9);
    assert(leafPBTreeNodes[0].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] == -1);

    std::cout << "Leaf level build test passed!" << std::endl;
}

void testBuildLeafLevelMedium()
{
    bool verbose = true;
    std::vector<int> gameEntryBlocks = std::vector<int>();
    for (int i = 0; i < MAX_INDEX_PER_BLOCK * 2 + 5; i++)
    {
        gameEntryBlocks.push_back(i * 10);
    }

    std::vector<BPTreeNode> leafPBTreeNodes = std::vector<BPTreeNode>();

    int offset = 0;
    buildLeafLevel(gameEntryBlocks, leafPBTreeNodes);

    if (verbose)
    {
        for (int i = 0; i < leafPBTreeNodes.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "IndexBlock count: " << leafPBTreeNodes[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < leafPBTreeNodes[i].indexBlock->count; ++j)
            {
                std::cout << leafPBTreeNodes[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= leafPBTreeNodes[i].indexBlock->count; ++j)
            {
                std::cout << leafPBTreeNodes[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << leafPBTreeNodes[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
        std::cout << "Offset: " << offset << std::endl;
    }

    assert(leafPBTreeNodes.size() == 3);
    assert(leafPBTreeNodes[0].indexBlock->count == MAX_INDEX_PER_BLOCK);
    assert(leafPBTreeNodes[0].indexBlock->keys[0] == 0);
    assert(leafPBTreeNodes[0].indexBlock->keys[MAX_INDEX_PER_BLOCK - 1] == (MAX_INDEX_PER_BLOCK - 1) * 10);
    assert(leafPBTreeNodes[0].indexBlock->childrenPtr[0] == 0);
    assert(leafPBTreeNodes[0].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] == 1);

    std::cout << "Leaf block 1 passed!" << std::endl;

    assert(leafPBTreeNodes[1].indexBlock->count == MAX_INDEX_PER_BLOCK);
    assert(leafPBTreeNodes[1].indexBlock->keys[0] == MAX_INDEX_PER_BLOCK * 10);
    assert(leafPBTreeNodes[1].indexBlock->keys[MAX_INDEX_PER_BLOCK - 1] == (MAX_INDEX_PER_BLOCK * 2 - 1) * 10);
    assert(leafPBTreeNodes[1].indexBlock->childrenPtr[0] == MAX_INDEX_PER_BLOCK);
    assert(leafPBTreeNodes[1].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] == -1);

    std::cout << "Leaf block 2 passed!" << std::endl;

    std::cout << "Medium Leaf level build test passed!" << std::endl;
}

void testBuildLeafLevelBIG()
{
    bool verbose = true;
    std::vector<int> gameEntryBlocks = std::vector<int>();
    for (int i = 0; i < MAX_INDEX_PER_BLOCK * 10; i++)
    {
        gameEntryBlocks.push_back(i * 10);
    }

    std::vector<BPTreeNode> leafPBTreeNodes = std::vector<BPTreeNode>();

    int offset = 0;
    buildLeafLevel(gameEntryBlocks, leafPBTreeNodes);

    if (verbose)
    {
        for (int i = 0; i < leafPBTreeNodes.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "IndexBlock count: " << leafPBTreeNodes[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < leafPBTreeNodes[i].indexBlock->count; ++j)
            {
                std::cout << leafPBTreeNodes[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= leafPBTreeNodes[i].indexBlock->count; ++j)
            {
                std::cout << leafPBTreeNodes[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << leafPBTreeNodes[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
        std::cout << "Offset: " << offset << std::endl;
    }

    std::cout << "No assertion for this test, please check the output" << std::endl;
    std::cout << "BIG Leaf level build test passed!" << std::endl;
}

IndexBlock *getDummyIndexBlock(int blkNumber)
{
    IndexBlock *indexBlock = new IndexBlock();
    indexBlock->count = MAX_INDEX_PER_BLOCK;
    for (int i = 0; i < MAX_INDEX_PER_BLOCK; i++)
    {
        indexBlock->keys[i] = (blkNumber * MAX_INDEX_PER_BLOCK + i) * 10;
        indexBlock->childrenPtr[i] = blkNumber * MAX_INDEX_PER_BLOCK + i;
    }
    indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] = -1; // this is wrong but we are not using it in the test
    return indexBlock;
}

void testBuildIndexNodeSmall()
{
    bool verbose = true;
    std::vector<BPTreeNode> childrenPtrs = std::vector<BPTreeNode>();
    for (int i = 0; i < 2; i++)
    {
        BPTreeNode bpTreeNode = new BPTreeNode();
        IndexBlock *indexBlock = getDummyIndexBlock(i);
        bpTreeNode.indexBlock = indexBlock;
        bpTreeNode.minKey = indexBlock->keys[0];

        childrenPtrs.push_back(bpTreeNode);
    }
    IndexBlock *childrenPtr1 = childrenPtrs[0].indexBlock;
    IndexBlock *childrenPtr2 = childrenPtrs[1].indexBlock;

    if (verbose)
    {
        for (int i = 0; i < childrenPtrs.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "IndexBlock count: " << childrenPtrs[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < childrenPtrs[i].indexBlock->count; ++j)
            {
                std::cout << childrenPtrs[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= childrenPtrs[i].indexBlock->count; ++j)
            {
                std::cout << childrenPtrs[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << childrenPtrs[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
    }

    std::vector<BPTreeNode> parentBlocks = std::vector<BPTreeNode>();

    int offset = 0;
    buildLevel(offset, childrenPtrs, parentBlocks);

    if (verbose)
    {
        std::cout << "=== Parent blocks: ===" << std::endl;
        for (int i = 0; i < parentBlocks.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "IndexBlock count: " << parentBlocks[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < parentBlocks[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= parentBlocks[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << parentBlocks[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
        std::cout << "Offset: " << offset << std::endl;
    }
    std::cout << "No assertion for this test, please check the output" << std::endl;
    std::cout << "Small Index level build test passed!" << std::endl;
}

void testBuildIndexNodeMedium()
{
    bool verbose = true;
    std::vector<BPTreeNode> childrenPtrs = std::vector<BPTreeNode>();
    for (int i = 0; i < (MAX_INDEX_PER_BLOCK + 1) * 2 - 3; i++)
    {
        BPTreeNode bpTreeNode = BPTreeNode();
        IndexBlock *indexBlock = getDummyIndexBlock(i);
        bpTreeNode.indexBlock = indexBlock;
        bpTreeNode.minKey = indexBlock->keys[0];

        childrenPtrs.push_back(bpTreeNode);
    }

    if (verbose)
    {
        for (int i = 0; i < childrenPtrs.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "IndexBlock count: " << childrenPtrs[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < childrenPtrs[i].indexBlock->count; ++j)
            {
                std::cout << childrenPtrs[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= childrenPtrs[i].indexBlock->count; ++j)
            {
                std::cout << childrenPtrs[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << childrenPtrs[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
    }

    std::vector<BPTreeNode> parentBlocks = std::vector<BPTreeNode>();

    int offset = 0;
    buildLevel(offset, childrenPtrs, parentBlocks);

    if (verbose)
    {
        std::cout << "=== Parent blocks: ===" << std::endl;
        for (int i = 0; i < parentBlocks.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "IndexBlock count: " << parentBlocks[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < parentBlocks[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= parentBlocks[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << parentBlocks[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
        std::cout << "Offset: " << offset << std::endl;
    }
    std::cout << "No assertion for this test, please check the output" << std::endl;
    std::cout << "Medium Index level build test passed!" << std::endl;
}

void testBuildIndexNodeBIGBIGBIG()
{
    bool verbose = true;
    int currBlock = 0;
    std::vector<BPTreeNode> childrenPtrs = std::vector<BPTreeNode>();
    for (int i = 0; i < (MAX_INDEX_PER_BLOCK + 1) * (MAX_INDEX_PER_BLOCK + 1) * 2; i++)
    {
        BPTreeNode bpTreeNode = BPTreeNode();
        IndexBlock *indexBlock = getDummyIndexBlock(i);
        bpTreeNode.indexBlock = indexBlock;
        bpTreeNode.minKey = indexBlock->keys[0];

        childrenPtrs.push_back(bpTreeNode);
    }

    if (verbose)
    {
        std::cout << "=== Children blocks: ===" << std::endl;
        std::cout << "Children blocks count: " << childrenPtrs.size() << std::endl;
    }
    currBlock = childrenPtrs.size();

    std::vector<BPTreeNode> parentBlocks = std::vector<BPTreeNode>();

    int offset = 0;
    buildLevel(offset, childrenPtrs, parentBlocks);
    offset += childrenPtrs.size();

    if (verbose)
    {
        std::cout << "=== Parent blocks lvl 1: ===" << std::endl;
        for (int i = 0; i < parentBlocks.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "Current Block: " << currBlock << std::endl;
            currBlock++;
            std::cout << "IndexBlock count: " << parentBlocks[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < parentBlocks[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= parentBlocks[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << parentBlocks[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
        std::cout << "Offset: " << offset << std::endl;
    }

    std::vector<BPTreeNode> parentBlocks2 = std::vector<BPTreeNode>();

    buildLevel(offset, parentBlocks, parentBlocks2);
    offset += parentBlocks.size();

    if (verbose)
    {
        std::cout << "=== Parent blocks lvl 2: ===" << std::endl;
        for (int i = 0; i < parentBlocks2.size(); i++)
        {
            std::cout << "Current node: " << i << std::endl;
            std::cout << "Current Block: " << currBlock << std::endl;
            currBlock++;
            std::cout << "IndexBlock count: " << parentBlocks2[i].indexBlock->count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < parentBlocks2[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks2[i].indexBlock->keys[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Children pointers: ";
            for (int j = 0; j <= parentBlocks2[i].indexBlock->count; ++j)
            {
                std::cout << parentBlocks2[i].indexBlock->childrenPtr[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Last Ptr " << parentBlocks2[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
        }
        std::cout << "Offset: " << offset << std::endl;
    }

    std::cout << "BIGBIGBIG Index level build test passed!" << std::endl;
}

void verbosePrinter(std::vector<BPTreeNode> bptreeNode, int offset)
{
    for (int i = 0; i < bptreeNode.size(); i++)
    {
        std::cout << "# Current node: " << i + offset << std::endl;
        std::cout << "IndexBlock count: " << bptreeNode[i].indexBlock->count << std::endl;
        std::cout << "Keys: ";
        for (int j = 0; j < bptreeNode[i].indexBlock->count; ++j)
        {
            std::cout << bptreeNode[i].indexBlock->keys[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Children pointers: ";
        for (int j = 0; j <= bptreeNode[i].indexBlock->count; ++j)
        {
            std::cout << bptreeNode[i].indexBlock->childrenPtr[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Last Ptr " << bptreeNode[i].indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
    }
}

void testBuildOneLeafAndOneLevel()
{
    bool verbose = true;
    bool superVerbose = true;
    std::vector<int> gameEntryBlocks = std::vector<int>();
    for (int i = 0; i < 100; i++)
    {
        gameEntryBlocks.push_back(i * 10);
    }

    std::vector<BPTreeNode> leafPBTreeNodes = std::vector<BPTreeNode>();

    int offset = 0;
    int depth = 0;
    buildLeafLevel(gameEntryBlocks, leafPBTreeNodes);

    std::vector<BPTreeNode> allBPTreeNodes = std::vector<BPTreeNode>();
    for (int i = 0; i < leafPBTreeNodes.size(); i++)
    {
        allBPTreeNodes.push_back(leafPBTreeNodes[i]);
    }

    if (superVerbose)
    {
        std::cout << "=== Leaf blocks: ===" << std::endl;
        verbosePrinter(leafPBTreeNodes, 0);
    }

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
        if (superVerbose)
        {
            std::cout << "=== Parent blocks " << depth << ": ===" << std::endl;
            verbosePrinter(parentBlocks, offset);
        }
    }

    if (verbose)
    {
        std::cout << "=== Root block: ===" << std::endl;
        BPTreeNode root = currentChildren[0];
        std::cout << "IndexBlock count: " << root.indexBlock->count << std::endl;
        std::cout << "Keys: ";
        for (int j = 0; j < root.indexBlock->count; ++j)
        {
            std::cout << root.indexBlock->keys[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Children pointers: ";
        for (int j = 0; j <= root.indexBlock->count; ++j)
        {
            std::cout << root.indexBlock->childrenPtr[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Last Ptr " << root.indexBlock->childrenPtr[MAX_INDEX_PER_BLOCK] << std::endl;
    }

    std::cout << "One leaf and one level build test passed!" << std::endl;
}

void testBuildBPTree()
{
    bool verbose = true;
    std::vector<GameEntryBlock> gameEntryBlocks = std::vector<GameEntryBlock>();
    for (int i = 0; i < MAX_INDEX_PER_BLOCK * 100; i++)
    {
        GameEntryBlock block = GameEntryBlock();
        block.count = 0;
        for (int j = 0; j < MAX_ENTRIES_PER_BLOCK; j++)
        {
            GameEntry entry;
            entry.FG_PCT_home = (i * MAX_ENTRIES_PER_BLOCK + j) * 10;
            block.entries[j] = entry;
            block.count++;
        }
        gameEntryBlocks.push_back(block);
    }

    if (verbose)
    {
        std::cout << "=== Game Entry Blocks: ===" << std::endl;
        for (int i = 0; i < gameEntryBlocks.size(); i++)
        {
            std::cout << "Current block: " << i << std::endl;
            std::cout << "Count: " << gameEntryBlocks[i].count << std::endl;
            std::cout << "Keys: ";
            for (int j = 0; j < gameEntryBlocks[i].count; ++j)
            {
                std::cout << gameEntryBlocks[i].entries[j].FG_PCT_home << " ";
            }
            std::cout << std::endl;
        }
    }

    std::vector<BPTreeNode> allBPTreeNodes = std::vector<BPTreeNode>();
    int depth = 0;
    int rootIndex = 0;
    buildBPTree(gameEntryBlocks, allBPTreeNodes, &depth, &rootIndex);

    if (verbose)
    {
        std::cout << "=== All B+ Tree Nodes: ===" << std::endl;
        verbosePrinter(allBPTreeNodes, 0);

        std::cout << "Depth: " << depth << std::endl;
        std::cout << "Root index: " << rootIndex << std::endl;
    }
}

void testBuildLeafLevel()
{
    // testBuildLeafLevelSmall();
    testBuildLeafLevelMedium();
    // testBuildLeafLevelBIG();
}

void testBuildIndexLevel()
{
    // testBuildIndexNodeSmall();
    // testBuildIndexNodeMedium();
    // testBuildIndexNodeBIGBIGBIG();
}

void dummy()
{
    std::fstream file("testBulkWriteToStorage", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Failed to create temporary file" << std::endl;
        return;
    }

    // Create a storage object with the temporary file
    Storage storage = Storage(&file);

    char *blockData = new char[BLOCK_SIZE];
    storage.readBlock(blockData, 116);
    blockData;
}

int main()
{
    // testBPTree();
    testBulkWriteToStorage();
    // dummy();

    // testBuildLeafLevel();
    // testBuildIndexLevel();
    // testBuildOneLeafAndOneLevel();
    // testBuildBPTree();
    return 0;
}