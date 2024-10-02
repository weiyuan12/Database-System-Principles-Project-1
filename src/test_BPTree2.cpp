#include <iostream>
#include <stdio.h>
#include <fstream>
#include "BPTree.cpp"
#include "DataFile.cpp"
#include "DataFileReader.cpp"
// #include "Storage.cpp"
#include <cassert>

void buildDB()
{
    // Create new database files
    std::fstream indexFile("index.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

    if (!indexFile.is_open() || !entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    Storage indexStorage = Storage(&indexFile);
    Storage entriesStorage = Storage(&entriesFile);

    DataFile dataFile = DataFile(&entriesStorage);

    // DataFileReader reader("../data/games.txt");
    // DataFileReader reader("../data/game_short_duplicate.txt");
    DataFileReader reader("../data/game_short.txt");
    std::vector<GameEntry> games = reader.readData();

    std::stable_sort(games.begin(), games.end(), [](const GameEntry &a, const GameEntry &b)
                     { return a.FG_PCT_home < b.FG_PCT_home; });

    std::vector<GameEntryBlock> gameEntriesBlocks = std::vector<GameEntryBlock>();
    GameEntriesToBlocks(games, gameEntriesBlocks);

    dataFile.writeAllGameEntryBlocks(gameEntriesBlocks);

    std::vector<BPTreeNode> allBPTreeNodes = std::vector<BPTreeNode>();
    int depth = 0;
    int rootIndex = 0;
    buildBPTree(gameEntriesBlocks, allBPTreeNodes, &depth, &rootIndex);

    bptreeBlocksToStorage(allBPTreeNodes, depth, rootIndex, &indexStorage);
}

void makeRangeQuery(int startKey, int endKey)
{
    std::fstream indexFile("index.dat", std::ios::binary | std::ios::in | std::ios::out);
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out);

    if (!indexFile.is_open() || !entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    Storage indexStorage = Storage(&indexFile);
    Storage entriesStorage = Storage(&entriesFile);

    BPTree bptree = BPTree(&indexStorage);
    DataFile dataFile = DataFile(&entriesStorage);

    std::vector<int> result;
    bptree.findRange(startKey, endKey, result);
    std::cout << "count: " << result.size() << std::endl;
    for (int i : result)
    {
        std::cout << "Index: " << i << std::endl;
    }

    for (int i = 0; i < result.size(); i++)
    {
        GameEntryBlock block;
        int blockIndex = result[i] / MAX_ENTRIES_PER_BLOCK;
        int positionInBlock = result[i] % MAX_ENTRIES_PER_BLOCK;

        dataFile.readGameEntryBlock(&block, blockIndex);
        const GameEntry &entry = block.entries[positionInBlock];

        std::cout << entry.AST_home
                  << " " << entry.FG_PCT_home
                  << " " << entry.FT_PCT_home
                  << " " << entry.FG3_PCT_home
                  << " " << entry.REB_home
                  << " " << entry.PTS_home
                  << " " << entry.TEAM_ID_home
                  << " " << entry.HOME_TEAM_WINS
                  << std::endl;
    }

    std::cout << "Total data blocks read: " << entriesStorage.getReadCount() << std::endl;
    std::cout << "Total index blocks read: " << indexStorage.getReadCount() << std::endl;
}

void makeRangeQuery(int startKey, int endKey, std::vector<GameEntry> *gameEntries, int *dataStorageCalls, int *indexStorageCalls)
{
    std::fstream indexFile("index.dat", std::ios::binary | std::ios::in | std::ios::out);
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out);

    if (!indexFile.is_open() || !entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    Storage indexStorage = Storage(&indexFile);
    Storage entriesStorage = Storage(&entriesFile);

    BPTree bptree = BPTree(&indexStorage);
    DataFile dataFile = DataFile(&entriesStorage);

    std::vector<int> result;
    bptree.findRange(startKey, endKey, result);

    for (int i = 0; i < result.size(); i++)
    {
        GameEntryBlock block;
        int blockIndex = result[i] / MAX_ENTRIES_PER_BLOCK;
        int positionInBlock = result[i] % MAX_ENTRIES_PER_BLOCK;

        dataFile.readGameEntryBlock(&block, blockIndex);
        GameEntry &entry = block.entries[positionInBlock];

        gameEntries->push_back(entry);
    }

    *dataStorageCalls = entriesStorage.getReadCount();
    *indexStorageCalls = indexStorage.getReadCount();
}

int main()
{
    buildDB();
    makeRangeQuery(429, 430);
    std::vector<GameEntry> gameEntries1;
    int dataStorageCalls = 0;
    int indexStorageCalls = 0;

    makeRangeQuery(429, 430, &gameEntries1, &dataStorageCalls, &indexStorageCalls);
    std::cout << "Total Game Entries: " << gameEntries1.size() << std::endl;
    std::cout << "Data Storage Calls: " << dataStorageCalls << " | expected 275" << std::endl;
    std::cout << "Index Storage Calls: " << indexStorageCalls << " | expected 9" << std::endl;
    assert(gameEntries1.size() == 275);
    assert(dataStorageCalls == 275);
    assert(indexStorageCalls == 9);
    std::cout << "Test passed" << std::endl;

    return 0;
}