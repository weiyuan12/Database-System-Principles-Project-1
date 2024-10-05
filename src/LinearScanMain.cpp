#include <iostream>
#include <stdio.h>
#include <fstream>
#include "BPTree.cpp"
#include "DataFile.cpp"
#include "DataFileReader.cpp"
#include <cassert>

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

    // DataFileReader reader("../data/games.txt");
    // DataFileReader reader("../data/game_short_duplicate.txt");
    DataFileReader reader("../data/game_short.txt");
    std::vector<GameEntry> games = reader.readData();

    std::stable_sort(games.begin(), games.end(), [](const GameEntry &a, const GameEntry &b)
                     { return a.FG_PCT_home < b.FG_PCT_home; });

    std::vector<GameEntryBlock> gameEntriesBlocks = std::vector<GameEntryBlock>();
    GameEntriesToBlocks(games, gameEntriesBlocks);

    dataFile.writeAllGameEntryBlocks(gameEntriesBlocks);
}

void linearScan(float startKey, float endKey, int *totalEntries)
{
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out);

    if (!entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    Storage entriesStorage = Storage(&entriesFile);

    DataFile dataFile = DataFile(&entriesStorage);

    std::cout << "Reading all game entries" << std::endl;
    std::vector<GameEntry> gameEntries = std::vector<GameEntry>();
    dataFile.readAllGameEntries(&gameEntries);

    for (const auto &entry : gameEntries)
    {
        std::cout << "Game Entry TEAM_ID_home: " << entry.TEAM_ID_home << ", FG_PCT_home: " << entry.FG_PCT_home << std::endl;
    }
    *totalEntries = gameEntries.size();

    std::cout << "Game Entries: " << gameEntries.size() << std::endl;
    std::vector<GameEntry> filteredEntries = std::vector<GameEntry>();
    int count = 0;
    for (const auto &entry : gameEntries)
    {
        if (entry.FG_PCT_home >= startKey && entry.FG_PCT_home <= endKey)
        {
            filteredEntries.push_back(entry);
            count++;
        }
    }

    if (true)
    {
        for (const auto &entry : filteredEntries)
        {
            std::cout << "Game Entry TEAM_ID_home: " << entry.TEAM_ID_home << ", FG_PCT_home: " << entry.FG_PCT_home << std::endl;
        }
    }
    std::cout << "Filtered Game Entries: " << count << std::endl;
    std::cout << "Total Game Entries: " << filteredEntries.size() << std::endl;
    std::cout << "Data Storage Fetched: " << entriesStorage.getFetchedCount() << " for " << entriesStorage.getReadCount() << " reads" << std::endl;
}

int main()
{
    buildDataFile();
    int totalEntries = 0;
    linearScan(0.5, 0.8, &totalEntries);
    return 0;
}