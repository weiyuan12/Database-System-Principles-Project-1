#include <cstdio>
#include <cstring> // Add this line to include the <cstring> header file
#include "./Storage.cpp"
#include "./constants.h"
#include <vector>

#ifndef DATAFILE_H
#define DATAFILE_H

class DataFile
{
private:
    Storage *storage;

public:
    void writeBlock(int blockNumber, GameEntryBlock *block);
    // void readGameEntryBlock(GameEntryBlock *gameEntryBlock, int blockNumber);
    // GameEntryBlock *readGameEntryBlock(int blockNumber);
    void addGameEntryBlock(GameEntryBlock *block);
    void readGameEntryBlock(GameEntryBlock *gameEntryBlock, int blockNumber);
    void readAllGameEntries(std::vector<GameEntry> *blocks);
    void writeGameEntryBlock(int blockNumber, GameEntryBlock *block);
    void writeAllGameEntryBlocks(std::vector<GameEntryBlock> &blocks);
    void writeAllGameEntries(std::vector<GameEntry> &blocks);
    DataFile(std::fstream *file);
    DataFile(Storage *storage);
    ~DataFile();
};

// GameEntryBlock *DataFile::readGameEntryBlock(int blockNumber)
// {
//     char *blockData = new char[BLOCK_SIZE];
//     storage->readBlock(blockData, blockNumber);
//     GameEntryBlock *block = reinterpret_cast<GameEntryBlock *>(blockData);
//     return block;
// }

void DataFile::readGameEntryBlock(GameEntryBlock *gameEntryBlock, int blockNumber)
{
    storage->readBlock(reinterpret_cast<char *>(gameEntryBlock), blockNumber);
}

void DataFile::readAllGameEntries(std::vector<GameEntry> *blocks)
{
    int totalBlocks = storage->getNumberOfBlocks();
    for (int i = 0; i < totalBlocks; i++)
    {
        GameEntryBlock block;
        readGameEntryBlock(&block, i);
        for (int j = 0; j < block.count; j++)
        {
            blocks->push_back(block.entries[j]);
        }
    }
}

void DataFile::addGameEntryBlock(GameEntryBlock *block)
{
    char *blockData = reinterpret_cast<char *>(block);
    storage->addBlock(blockData);
}

// This is probably not needed
void DataFile::writeGameEntryBlock(int blockNumber, GameEntryBlock *block)
{
    char *blockData = reinterpret_cast<char *>(block);
    storage->writeBlock(blockNumber, blockData);
}

void DataFile::writeAllGameEntryBlocks(std::vector<GameEntryBlock> &blocks)
{
    for (int i = 0; i < blocks.size(); i++)
    {
        addGameEntryBlock(&blocks[i]);
    }
}

void GameEntriesToBlocks(std::vector<GameEntry> &blocks, std::vector<GameEntryBlock> &gameEntryBlocks)
{
    int t = blocks.size();
    // Fast cieling division
    int totalBlocks = (blocks.size() + MAX_ENTRIES_PER_BLOCK - 1) / MAX_ENTRIES_PER_BLOCK;

    for (int i = 0; i < totalBlocks; i++)
    {
        GameEntryBlock block;
        block.count = 0;

        // Copy entries to the current block
        while (block.count < MAX_ENTRIES_PER_BLOCK && i * MAX_ENTRIES_PER_BLOCK + block.count < blocks.size())
        {
            memcpy(&block.entries[block.count], &blocks[i * MAX_ENTRIES_PER_BLOCK + block.count], sizeof(GameEntry));
            ++block.count;
        }

        gameEntryBlocks.push_back(block);
    }
}

void DataFile::writeAllGameEntries(std::vector<GameEntry> &blocks)
{
    std::vector<GameEntryBlock> gameEntryBlocks;
    GameEntriesToBlocks(blocks, gameEntryBlocks);
    writeAllGameEntryBlocks(gameEntryBlocks);
}

DataFile::DataFile(std::fstream *file)
{
    storage = new Storage(file);
}

DataFile::DataFile(Storage *storage)
{
    this->storage = storage;
}

DataFile::~DataFile()
{
    delete storage;
}

#endif // DATAFILE_H;
