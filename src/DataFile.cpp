#include <cstdio>
#include <cstring> // Add this line to include the <cstring> header file
#include "./Storage.cpp"
#include "./constants.h"

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
    void writeGameEntryBlock(int blockNumber, GameEntryBlock *block);
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
