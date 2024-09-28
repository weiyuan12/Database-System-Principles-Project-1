
#include "./BPTree.cpp"
#include "./DataFile.cpp"
#include "./constants.h"
#include "./DataFileReader.cpp"
#include <fstream>
#include <iostream>

int main()
{

    std::fstream indexFile("index.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

    if (!indexFile.is_open() || !entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return 1;
    }

    Storage indexStorage = Storage(&indexFile);
    Storage entriesStorage = Storage(&entriesFile);

    DataFile dataFile = DataFile(&entriesStorage);

    // DataFileReader reader("../data/games.txt");
    DataFileReader reader("../data/game_short.txt");
    std::vector<GameEntry> games = reader.readData();

    std::sort(games.begin(), games.end(), [](const GameEntry &a, const GameEntry &b)
              { return a.FG_PCT_home < b.FG_PCT_home; });

    std::vector<GameEntryBlock> gameEntriesBlocks = std::vector<GameEntryBlock>();
    GameEntriesToBlocks(games, gameEntriesBlocks);

    dataFile.writeAllGameEntryBlocks(gameEntriesBlocks);

    std::vector<BPTreeNode> allBPTreeNodes = std::vector<BPTreeNode>();
    int depth = 0;
    int rootIndex = 0;
    buildBPTree(gameEntriesBlocks, allBPTreeNodes, &depth, &rootIndex);

    bptreeBlocksToStorage(allBPTreeNodes, depth, rootIndex, &indexStorage);

    std::cout << "B+ Tree depth: " << depth << std::endl;
    std::cout << "B+ Tree root index: " << rootIndex << std::endl;
    std::cout << "Build B+ Tree successful!" << std::endl;
}
