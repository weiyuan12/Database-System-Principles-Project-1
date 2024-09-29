#include <cstdio>
#include <fstream>
#include <iostream>
#ifndef STORAGE_H
#define STORAGE_H

#include "./constants.h"
#include <vector>
// #include <cassert>

class Storage
{
private:
    /* data */
    std::fstream *ptr;
    int readCount = 0;

public:
    Storage(std::fstream *fileHandle);
    // Constructor taking a file handle
    void addBlock(char *blockData);
    ~Storage();
    void deleteBlock(char *block);
    void readBlock(char *readto, int blockNumber);
    int getReadCount();
    void writeBlock(int blockNumber, const char *blockData);
    void bulkWrite(std::vector<char *> &blocks);
};

Storage::Storage(std::fstream *fileHandle)
{
    // assert(fileHandle->flags() & std::ios::binary); // Does not work
    ptr = fileHandle;
}

void Storage::addBlock(char *blockData)
{
    ptr->seekg(0, std::ios::end);      // Move the file pointer to the end of the file
    ptr->write(blockData, BLOCK_SIZE); // Write the block data to the file
    ptr->flush();                      // Flush the file buffer to ensure the data is written immediately
}

void Storage::readBlock(char *readTo, int blockNumber)
{
    readCount++;
    // Calculate the position of the block
    long int position = blockNumber * BLOCK_SIZE;

    // Move the file pointer to the position of the block
    ptr->seekp(position, std::ios::beg);

    ptr->read(readTo, BLOCK_SIZE);
}

int Storage::getReadCount()
{
    return readCount;
}

void Storage::writeBlock(int blockNumber, const char *blockData)
{
    // Calculate the position of the block
    long int position = blockNumber * BLOCK_SIZE;

    // Move the file pointer to the position of the block
    ptr->seekp(position, std::ios::beg);

    // Write the block data to the file
    ptr->write(blockData, BLOCK_SIZE);

    // Flush the file buffer to ensure the data is written immediately
    ptr->flush();
}

void Storage::bulkWrite(std::vector<char *> &blocks)
{
    for (int i = 0; i < blocks.size(); i++)
    {
        if (strlen(blocks[i]) != BLOCK_SIZE)
        {
            std::cerr << "Error: Block " << i << " is not " << BLOCK_SIZE << " bytes." << std::endl;
            continue; // Skip this block
        }
        addBlock(blocks[i]);
    }
}

Storage::~Storage()
{
    // Close the file handle
    ptr->close();
}
#endif // STORAGE_H
