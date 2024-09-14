#include <cstdio>
#include <fstream>
#ifndef STORAGE_H
#define STORAGE_H

#include "./constants.h"

class Storage
{
private:
    /* data */
    std::fstream *ptr;

public:
    Storage(std::fstream *fileHandle);
    // Constructor taking a file handle
    char *addBlock(const char *blockData);
    ~Storage();
    void deleteBlock(char *block);
    char *readBlock(int blockNumber);
};

Storage::Storage(std::fstream *fileHandle)
{
    ptr = fileHandle;
}

char *Storage::addBlock(const char *blockData)
{
    ptr->seekg(0, std::ios::end);      // Move the file pointer to the end of the file
    ptr->write(blockData, BLOCK_SIZE); // Write the block data to the file
    ptr->flush();                      // Flush the file buffer to ensure the data is written immediately

    // Calculate the position of the added block
    long int position = static_cast<long int>(ptr->tellg()) - BLOCK_SIZE;

    // Allocate memory for the block
    char *addedBlock = new char[BLOCK_SIZE];

    // Move the file pointer to the position of the added block
    ptr->seekg(position, std::ios::beg);

    // Read the added block from the file
    ptr->read(addedBlock, BLOCK_SIZE);

    // Move the file pointer back to the end of the file
    ptr->seekg(0, std::ios::end);

    // Return the pointer to the added block
    return addedBlock;
}

/**
 * @brief Reads a block of data from the storage file.
 *
 * @param blockNumber The number of the block to be read.
 * @return A pointer to the block data.
 */
char *Storage::readBlock(int blockNumber)
{
    // Calculate the position of the block
    long int position = blockNumber * BLOCK_SIZE;

    // Move the file pointer to the position of the block
    ptr->seekg(position, std::ios::beg);

    // Allocate memory for the block
    char *blockData = new char[BLOCK_SIZE];

    // Read the block from the file
    ptr->read(blockData, BLOCK_SIZE);

    // Return the pointer to the block
    return blockData;
}

void Storage::deleteBlock(char *block)
{
    // This function is not needed for the project
    // Add documentation here explaining why it is not needed
}

Storage::~Storage()
{
    // Close the file handle
    ptr->close();

    // Delete the file handle
    delete ptr;
}

#endif // STORAGE_H
