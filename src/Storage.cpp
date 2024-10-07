#include <cstdio>
#include <fstream>
#include <iostream>
#ifndef STORAGE_H
#define STORAGE_H

#include "./constants.h"
#include <vector>
#include <unordered_map>
// #include <cassert>

class Storage
{
private:
    /* data */
    std::fstream *ptr;
    int blocksFetched = 0;
    int blocksRead = 0;
    std::unordered_map<int, char *> blocks;
    const int CACHE_SIZE = 5;

public:
    Storage(std::fstream *fileHandle);
    // Constructor taking a file handle
    void addBlock(char *blockData);
    ~Storage();
    void deleteBlock(char *block);
    void readBlock(char *readto, int blockNumber);
    int getNumberOfBlocks();
    int getFetchedCount();
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
    blocksRead++;
    // Check if the block is in the cache
    if (blocks.find(blockNumber) != blocks.end())
    {
        // Block is in the cache, copy it to readTo
        std::memcpy(readTo, blocks[blockNumber], BLOCK_SIZE);
    }
    else
    {
        // Block is not in the cache, read it from the file
        // Calculate the position of the block
        long int position = blockNumber * BLOCK_SIZE;

        // Move the file pointer to the position of the block
        ptr->seekg(position, std::ios::beg);

        // Read the block data from the file
        ptr->read(readTo, BLOCK_SIZE);

        // Add the block to the cache
        char *cachedBlock = new char[BLOCK_SIZE];
        std::memcpy(cachedBlock, readTo, BLOCK_SIZE);
        blocks[blockNumber] = cachedBlock;
        blocksFetched++;

        // If the cache size exceeds CACHE_SIZE, invalidate a random block
        if (blocks.size() > CACHE_SIZE)
        {
            auto it = blocks.begin();
            std::advance(it, rand() % blocks.size());
            delete[] it->second;
            blocks.erase(it);
        }
    }
}

int Storage::getNumberOfBlocks()
{
    ptr->seekg(0, std::ios::end);
    int size = ptr->tellg();
    return size / BLOCK_SIZE;
}

int Storage::getFetchedCount()
{
    return blocksFetched;
}

int Storage::getReadCount()
{
    return blocksRead;
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
