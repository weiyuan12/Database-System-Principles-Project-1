#include <cstdio>
#include <fstream>
#include <iostream>
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
    void addBlock(const char *blockData);
    ~Storage();
    void deleteBlock(char *block);
    void readBlock(char *readto, int blockNumber);
    void writeBlock(int blockNumber, const char *blockData);
};

Storage::Storage(std::fstream *fileHandle)
{
    ptr = fileHandle;
}

void Storage::addBlock(const char *blockData)
{
    ptr->seekg(0, std::ios::end);      // Move the file pointer to the end of the file
    ptr->write(blockData, BLOCK_SIZE); // Write the block data to the file
    ptr->flush();                      // Flush the file buffer to ensure the data is written immediately
}

void Storage::readBlock(char *readTo, int blockNumber)
{
    // Calculate the position of the block
    long int position = blockNumber * BLOCK_SIZE;

    // Move the file pointer to the position of the block
    ptr->seekp(position, std::ios::beg);

    ptr->read(readTo, BLOCK_SIZE);
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

Storage::~Storage()
{
    // Close the file handle
    ptr->close();
}
#endif // STORAGE_H
