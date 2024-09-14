#include <stdio.h>
#include <fstream> // Add this line
#include <cstring> // Add this line for std::memcmp
#include "Storage.cpp"
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
    const char blockData[BLOCK_SIZE] = "This is a test block of data.";

    // Add the block to the storage
    char *addedBlock = storageObj.addBlock(blockData);

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
    delete[] addedBlock;
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
    const char blockData1[BLOCK_SIZE] = "This is the first test block of data.";
    const char blockData2[BLOCK_SIZE] = "This is the second test block of data.";
    const char blockData3[BLOCK_SIZE] = "This is the third test block of data.";

    // Add the blocks to the storage
    char *addedBlock1 = storageObj.addBlock(blockData1);
    char *addedBlock2 = storageObj.addBlock(blockData2);
    char *addedBlock3 = storageObj.addBlock(blockData3);

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
    delete[] addedBlock1;
    delete[] addedBlock2;
    delete[] addedBlock3;
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
    const char blockData[BLOCK_SIZE] = "This is a test block of data for persistent file.";

    // Add the block to the storage
    char *addedBlock = storageObj.addBlock(blockData);

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
    delete[] addedBlock;
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
    const char blockData1[BLOCK_SIZE] = "This is the first test block of data.";
    const char blockData2[BLOCK_SIZE] = "This is the second test block of data.";
    const char blockData3[BLOCK_SIZE] = "This is the third test block of data.";

    // Add the blocks to the storage
    char *addedBlock1 = storageObj.addBlock(blockData1);
    char *addedBlock2 = storageObj.addBlock(blockData2);
    char *addedBlock3 = storageObj.addBlock(blockData3);

    // Read the second block from the storage
    char *readBlock2 = storageObj.readBlock(1);

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
    delete[] addedBlock1;
    delete[] addedBlock2;
    delete[] addedBlock3;
    delete[] readBlock2;

    file.close();
    std::remove("testReadingSecondBlock");
}

int main()
{
    std::cout << "Running storage tests..." << std::endl;
    testAddBlock();
    testAddMultipleBlocks();
    // testAddBlockWithPersistentFile();
    testReadingSecondBlock();
    return 0;
}
