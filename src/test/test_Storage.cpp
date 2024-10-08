#include <stdio.h>
#include <fstream> // Add this line
#include <cstring> // Add this line for std::memcmp
#include "../Storage.cpp"
#include <iostream>

void testAddBlock()
{
    // Create a temporary file for testing
    std::fstream file("testAddBlock", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Failed to create temporary file" << std::endl;
        return;
    }

    // Create a storage object with the temporary file
    Storage storageObj = Storage(&file);

    // Define a block of data to add
    char blockData[BLOCK_SIZE] = "This is a test block of data.";

    // Add the block to the storage
    storageObj.addBlock(blockData);

    char addedBlock[BLOCK_SIZE];
    storageObj.readBlock(addedBlock, 0);

    // Verify that the added block matches the original block data
    if (std::memcmp(addedBlock, blockData, BLOCK_SIZE) == 0)
    {
        std::cout << "Test passed: Block data matches" << std::endl;
    }
    else
    {
        std::cerr << "Test failed: Block data does not match" << std::endl;
    }

    // Clean up
    file.close();
    std::remove("testAddBlock");
}

void testAddMultipleBlocks()
{
    std::fstream file("testAddMultipleBlocks", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Failed to create temporary file" << std::endl;
        return;
    }

    // Create a storage object with the temporary file
    Storage storageObj = Storage(&file);

    // Define multiple blocks of data to add
    char blockData1[BLOCK_SIZE] = "This is the first test block of data.";
    char blockData2[BLOCK_SIZE] = "This is the second test block of data.";
    char blockData3[BLOCK_SIZE] = "This is the third test block of data.";

    // Add the blocks to the storage
    storageObj.addBlock(blockData1);
    storageObj.addBlock(blockData2);
    storageObj.addBlock(blockData3);

    char addedBlock1[BLOCK_SIZE];
    char addedBlock2[BLOCK_SIZE];
    char addedBlock3[BLOCK_SIZE];

    storageObj.readBlock(addedBlock1, 0);
    storageObj.readBlock(addedBlock2, 1);
    storageObj.readBlock(addedBlock3, 2);

    // Verify that the added blocks match the original block data
    if (std::memcmp(addedBlock1, blockData1, BLOCK_SIZE) == 0 &&
        std::memcmp(addedBlock2, blockData2, BLOCK_SIZE) == 0 &&
        std::memcmp(addedBlock3, blockData3, BLOCK_SIZE) == 0)
    {
        std::cout << "Test passed: All block data matches" << std::endl;
    }
    else
    {
        std::cerr << "Test failed: Block data does not match" << std::endl;
    }

    // Clean up
    file.close();
    std::remove("testAddMultipleBlocks");
}

void testAddBlockWithPersistentFile()
{
    // Create a temporary file for testing
    std::fstream file("testAddBlockWithPersistentFile", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Failed to create temporary file" << std::endl;
        return;
    }

    // Create a storage object with the temporary file
    Storage storageObj = Storage(&file);

    // Define a block of data to add
    char blockData[BLOCK_SIZE] = "This is a test block of data for persistent file.";

    // Add the block to the storage
    storageObj.addBlock(blockData);

    // Close the file
    file.close();

    // Reopen the file
    file.open("testAddBlockWithPersistentFile", std::ios::binary | std::ios::in | std::ios::out);

    // Create a new storage object with the reopened file
    Storage storageObj2 = Storage(&file);

    char addedBlock[BLOCK_SIZE];
    storageObj2.readBlock(addedBlock, 0);

    // Verify that the added block matches the original block data
    if (std::memcmp(addedBlock, blockData, BLOCK_SIZE) == 0)
    {
        std::cout << "Test passed: Block data matches for persistent file" << std::endl;
    }
    else
    {
        std::cerr << "Test failed: Block data does not match for persistent file" << std::endl;
    }

    // Clean up
    file.close();
}

void testReadingSecondBlock()
{
    // Create a temporary file for testing
    std::fstream file("testReadingSecondBlock", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Failed to create temporary file" << std::endl;
        return;
    }

    // Create a storage object with the temporary file
    Storage storageObj = Storage(&file);

    // Define multiple blocks of data to add
    char blockData1[BLOCK_SIZE] = "This is the first test block of data.";
    char blockData2[BLOCK_SIZE] = "This is the second test block of data.";
    char blockData3[BLOCK_SIZE] = "This is the third test block of data.";

    // Add the blocks to the storage
    storageObj.addBlock(blockData1);
    storageObj.addBlock(blockData2);
    storageObj.addBlock(blockData3);

    char readBlock2[BLOCK_SIZE];
    storageObj.readBlock(readBlock2, 1);

    // Verify that the read block matches the second block data
    if (std::memcmp(readBlock2, blockData2, BLOCK_SIZE) == 0)
    {
        std::cout << "Test passed: Read block data matches second block" << std::endl;
    }
    else
    {
        std::cerr << "Test failed: Read block data does not match second block" << std::endl;
    }

    // Clean up
    file.close();
    std::remove("testReadingSecondBlock");
}

int main()
{
    std::cout << "Running storage tests..." << std::endl;
    testAddBlock();
    testAddMultipleBlocks();
    testAddBlockWithPersistentFile();
    testReadingSecondBlock();
    return 0;
}
