#include <cstdio>

#include "./constants.h"

class Storage
{
private:
    /* data */
    FILE *ptr;

public:
    Storage(FILE *fileHandle); // Constructor taking a file handle
    char *addBlock(const char *blockData);
    ~Storage();
    void deleteBlock(char *block);
    char *readBlock(int blockNumber);
};

Storage::Storage(FILE *fileHandle)
{
    ptr = fileHandle;
}

char *Storage::addBlock(const char *blockData)
{
    fseek(ptr, 0, SEEK_END);                          // Move the file pointer to the end of the file
    fwrite(blockData, sizeof(char), BLOCK_SIZE, ptr); // Write the block data to the file
    fflush(ptr);                                      // Flush the file buffer to ensure the data is written immediately

    // Calculate the position of the added block
    long int position = ftell(ptr) - BLOCK_SIZE;

    // Allocate memory for the block
    char *addedBlock = new char[BLOCK_SIZE];

    // Move the file pointer to the position of the added block
    fseek(ptr, position, SEEK_SET);

    // Read the added block from the file
    fread(addedBlock, sizeof(char), BLOCK_SIZE, ptr);

    // Move the file pointer back to the end of the file
    fseek(ptr, 0, SEEK_END);

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
    fseek(ptr, position, SEEK_SET);

    // Allocate memory for the block
    char *blockData = new char[BLOCK_SIZE];

    // Read the block from the file
    fread(blockData, sizeof(char), BLOCK_SIZE, ptr);

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
    fclose(ptr); // Close the file handle
}
