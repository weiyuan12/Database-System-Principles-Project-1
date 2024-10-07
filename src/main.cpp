#include <iostream>
#include <stdio.h>
#include <fstream>
#include "BPTree.cpp"
#include "DataFile.cpp"
#include "DataFileReader.cpp"
#include <cassert>
#include <chrono>

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

    DataFileReader reader("../data/games.txt");
    // DataFileReader reader("../data/game_short_duplicate.txt");
    // DataFileReader reader("../data/game_short.txt");
    std::vector<GameEntry> games = reader.readData();
    std::cout << "-------Task 1 -------" << std::endl;
    std::cout << "[Task 1] Size of a record: " << sizeof(games[0]) << std::endl;
    std::cout << "[Task 1] Number of records from memory: " << games.size() << std::endl;
    std::stable_sort(games.begin(), games.end(), [](const GameEntry &a, const GameEntry &b)
                     {
                         bool isNaN_a = std::isnan(a.FG_PCT_home);
                         bool isNaN_b = std::isnan(b.FG_PCT_home);

                         if (isNaN_a && !isNaN_b)
                             return true; 
                         if (!isNaN_a && isNaN_b)
                             return false;

                         return a.FG_PCT_home < b.FG_PCT_home;
                     });
    // std::cout << "[dbg] Number of records after sorting: " << games.size() << std::endl;

    // for (const auto &entry : games)
    // {
    //     std::cout << "Game Entry TEAM_ID_home: " << entry.TEAM_ID_home << ", FG_PCT_home: " << entry.FG_PCT_home << std::endl;
    // }

    std::vector<GameEntryBlock> gameEntriesBlocks = std::vector<GameEntryBlock>();
    GameEntriesToBlocks(games, gameEntriesBlocks);
    std::cout << "[Task 1] Number of records in a block: " << gameEntriesBlocks[0].count << std::endl;
    dataFile.writeAllGameEntryBlocks(gameEntriesBlocks);

    std::vector<BPTreeNode> allBPTreeNodes = std::vector<BPTreeNode>();
    int depth = 0;
    int rootIndex = 0;
    buildBPTree(gameEntriesBlocks, allBPTreeNodes, &depth, &rootIndex);
    std::cout << "[Task 1] Number of Blocks needed: " << gameEntriesBlocks.size() << std::endl;
    std::cout << "[Task 1] Number of Records in last block: " << gameEntriesBlocks[gameEntriesBlocks.size() - 1].count << std::endl;
    int a = ((gameEntriesBlocks.size() - 1) * gameEntriesBlocks[0].count) + gameEntriesBlocks[gameEntriesBlocks.size() - 1].count;
    std::cout << "[Task 1] Total records in Blocks: " << a << std::endl;

    std::cout << "-------Task 2 -------" << std::endl;
    std::cout << "[Task 2] Parameter n (MAX_INDEX_PER_BLOCK): " << allBPTreeNodes[0].indexBlock->count << std::endl;
    std::cout << "[Task 2] Number of Nodes: " << allBPTreeNodes.size() << std::endl;
    std::cout << "[Task 2] Levels/Depth of BPTree: " << depth << std::endl;
    std::cout << "[Task 2] Content of root node" << std::endl;
    bptreeBlocksToStorage(allBPTreeNodes, depth, rootIndex, &indexStorage);
    BPTree bptree = BPTree(&indexStorage);
    bptree.printIndexBlock(rootIndex);
}

void makeRangeQuery(float startKey, float endKey, std::vector<GameEntry> *gameEntries, int *dataStorageFetched, int *indexStorageFetched, int *dataStorageRead, int *indexStorageRead, double * timeTaken)
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
    
    // std::cout << "Index Storage Blocks" << std::endl;
    for (int i = 1; i < indexStorage.getNumberOfBlocks(); i++)
    {
        // bptree.printIndexBlock(i);
    }
    std::vector<int> *result = new std::vector<int>();

    auto startBPTreeQuery = std::chrono::high_resolution_clock::now();
    bptree.findRange(startKey, endKey, result);
    for (int i = 0; i < result->size(); i++)
    {
        GameEntryBlock block = GameEntryBlock();
        int blockIndex = (*result)[i] / MAX_ENTRIES_PER_BLOCK;
        int positionInBlock = (*result)[i] % MAX_ENTRIES_PER_BLOCK;
        dataFile.readGameEntryBlock(&block, blockIndex);
        GameEntry entry = block.entries[positionInBlock];

        gameEntries->push_back(entry);
    }
    auto stopBPTreeQuery = std::chrono::high_resolution_clock::now();
    auto durationBPTreeQuery = std::chrono::duration_cast<std::chrono::microseconds>(stopBPTreeQuery - startBPTreeQuery);
    *timeTaken = durationBPTreeQuery.count();

    // std::cout << "All Entries: " << std::endl;
    // for (const auto &entry : *gameEntries)
    // {
    //     std::cout << "TEAM_ID_home: " << entry.TEAM_ID_home << ", FG_PCT_home: " << entry.FG_PCT_home << std::endl;
    // }

    *dataStorageFetched = entriesStorage.getFetchedCount();
    *indexStorageFetched = indexStorage.getFetchedCount();
    *dataStorageRead = entriesStorage.getReadCount();
    *indexStorageRead = indexStorage.getReadCount();
}

float calculateAverageFGPct(const std::vector<GameEntry>& gameEntries) {
    float totalFGPct = 0.0f;
    
    for (const GameEntry& entry : gameEntries) {
        totalFGPct += entry.FG_PCT_home;
    }
    return totalFGPct / gameEntries.size();
}

int main()
{
    buildDB();

    std::vector<GameEntry> gameEntries1 = std::vector<GameEntry>();
    int dataStoragedFetched = 0;
    int indexStorageFetched = 0;
    int dataStorageRead = 0;
    int indexStorageRead = 0;
    double timeTaken = 0;

    std::cout << "-------Task 3 -------" << std::endl;
    makeRangeQuery(0.5, 0.8, &gameEntries1, &dataStoragedFetched, &indexStorageFetched, &dataStorageRead, &indexStorageRead, &timeTaken);
    std::cout << "[Task 3] Total Game Entries Retrieved: " << gameEntries1.size() << std::endl;
    std::cout << "[Task 3] Data Blocks accessed: " << dataStoragedFetched << " for " << dataStorageRead << " entries retrieved" << std::endl;
    std::cout << "[Task 3] Index Storage Fetched: " << indexStorageFetched << " for " << indexStorageRead << " reads" << std::endl;
    float average = calculateAverageFGPct(gameEntries1);
    std::cout << "[Task 3] Avg FG_PCT_HOME: " << average << std::endl;
    std::cout << "[Task 3] Time taken by B+ Tree: " << timeTaken << " microseconds" << std::endl;

    // if (true)
    // {
    //     for (int i = 0; i < gameEntries1.size(); i++)
    //     {
    //         std::cout << "Game Entry TEAM_ID_home: " << gameEntries1[i].TEAM_ID_home
    //                   << ", FG_PCT_home: " << gameEntries1[i].FG_PCT_home << std::endl;
    //     }
    // }

    return 0;
}