#include <iostream>
#include <stdio.h>
#include <fstream>
#include "BPTree.cpp"
#include "DataFile.cpp"
#include "DataFileReader.cpp"
#include <cassert>
#include <chrono>

void buildDataFile()
{
    // Create new database files
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

    if (!entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    Storage entriesStorage = Storage(&entriesFile);
    DataFile dataFile = DataFile(&entriesStorage);

    DataFileReader reader("../data/games.txt");
    // DataFileReader reader("../data/game_short_duplicate.txt");
    // DataFileReader reader("../data/game_short.txt");
    std::vector<GameEntry> games = reader.readData();

    std::stable_sort(games.begin(), games.end(), [](const GameEntry &a, const GameEntry &b)
                     {
                         bool isNaN_a = std::isnan(a.FG_PCT_home);
                         bool isNaN_b = std::isnan(b.FG_PCT_home);

                         if (isNaN_a && !isNaN_b)
                             return true; // `a` is NaN, so it's "less than" `b`
                         if (!isNaN_a && isNaN_b)
                             return false; // `b` is NaN, so `a` is "greater than" `b`

                         return a.FG_PCT_home < b.FG_PCT_home; // Standard comparison if both are numbers
                     });

    std::vector<GameEntryBlock> gameEntriesBlocks = std::vector<GameEntryBlock>();
    GameEntriesToBlocks(games, gameEntriesBlocks);

    dataFile.writeAllGameEntryBlocks(gameEntriesBlocks);
}

void linearScan(float startKey, float endKey, int *totalEntries, double *timeTaken)
{
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out);

    if (!entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    Storage entriesStorage = Storage(&entriesFile);

    DataFile dataFile = DataFile(&entriesStorage);

    std::cout << "[Task 3] Linear Scan comparison" << std::endl;
    // std::vector<GameEntry> gameEntries = std::vector<GameEntry>();
    // dataFile.readAllGameEntries(&gameEntries);

    auto startLinearScanQuery = std::chrono::high_resolution_clock::now();
    std::vector<GameEntry> filteredEntries = std::vector<GameEntry>();
    int num = entriesStorage.getNumberOfBlocks();
    for(int i = 0; i<=num; i++){
        GameEntryBlock block = GameEntryBlock();
        dataFile.readGameEntryBlock(&block, i);
        //std::cout << "Block: " << i << std::endl;
        for (int j = 0; j<block.count; j++)
        {
            if (block.entries[j].FG_PCT_home >= startKey && block.entries[j].FG_PCT_home <= endKey)
            {
                //std::cout << "Game Entries: " << block.entries[j].FG_PCT_home << std::endl;
                filteredEntries.push_back(block.entries[j]);
            }
        }
    }
    auto stopLinearScanQuery = std::chrono::high_resolution_clock::now();
    auto durationLinearScanQuery = std::chrono::duration_cast<std::chrono::microseconds>(stopLinearScanQuery - startLinearScanQuery);
    *timeTaken = durationLinearScanQuery.count();

    // for (const auto &entry : gameEntries)
    // {
    //     std::cout << "Game Entry TEAM_ID_home: " << entry.TEAM_ID_home << ", FG_PCT_home: " << entry.FG_PCT_home << std::endl;
    // }
    // *totalEntries = gameEntries.size();

    // std::cout << "Game Entries: " << gameEntries.size() << std::endl;
    // std::vector<GameEntry> filteredEntries = std::vector<GameEntry>();

    // auto startLinearScanQuery = std::chrono::high_resolution_clock::now();
    // volatile int dummy = 0;
    // for (const auto &entry : gameEntries)
    // {
    //     if (entry.FG_PCT_home >= startKey && entry.FG_PCT_home <= endKey)
    //     {
    //         filteredEntries.push_back(entry);
    //         dummy += entry.FG_PCT_home;
    //     }
    // }
    // auto stopLinearScanQuery = std::chrono::high_resolution_clock::now();
    // auto durationLinearScanQuery = std::chrono::duration_cast<std::chrono::microseconds>(stopLinearScanQuery - startLinearScanQuery);
    // *timeTaken = durationLinearScanQuery.count();
    // std::cout << "[Task 3] Time taken for LinearScan: " << durationLinearScanQuery.count() << " microseconds" << std::endl;


    // if (true)
    // {
    //     for (const auto &entry : filteredEntries)
    //     {
    //         std::cout << "Game Entry TEAM_ID_home: " << entry.TEAM_ID_home << ", FG_PCT_home: " << entry.FG_PCT_home << std::endl;
    //     }
    // }
    std::cout << "[Task 3] Total Game Entries Retrieved: " << filteredEntries.size() << std::endl;
    std::cout << "[Task 3] Data blocks accessed: " << entriesStorage.getFetchedCount() - 1<< " for " << filteredEntries.size()<< " entries retrieved" << std::endl;
}

int main()
{
    buildDataFile();
    int totalEntries = 0;
    double timeTaken = 0;

    linearScan(0.5, 0.8, &totalEntries, &timeTaken);
    std::cout << "[Task 3] Time taken for LinearScan: " << timeTaken << " microseconds" << std::endl;
}