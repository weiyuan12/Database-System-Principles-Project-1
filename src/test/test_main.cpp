#include <fstream>
#include <iostream>
#include "./BPTree.cpp"
#include "./DataFile.cpp"

int main()
{
    std::fstream indexFile("index.dat", std::ios::binary | std::ios::in | std::ios::out);
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out);

    if (!indexFile.is_open() || !entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return 1;
    }

    Storage indexStorage = Storage(&indexFile);
    Storage entriesStorage = Storage(&entriesFile);

    BPTree bptree = BPTree(&indexStorage);
    DataFile dataFile = DataFile(&entriesStorage);

    // 0.484
    // 0.488
    // 0.482
    // 0.441
    // 0.429
    // 0.386
    // 0.643
    // 0.553
    // 0.466

    // int toFind[] = {386, 429, 441, 466, 482, 484, 488, 553, 643};
    int toFind[] = {484};
    // sorted: 386, 429, 441, 466, 482, 484, 488, 553, 643

    for (int i = 0; i < 1; i++)
    {
        std::cout << "Finding: " << toFind[i] << std::endl;
        int gameEntryPosition = bptree.find(toFind[i]);
        GameEntryBlock block;
        dataFile.readGameEntryBlock(&block, gameEntryPosition / MAX_ENTRIES_PER_BLOCK);
        std::cout << "Game Entry Position: " << gameEntryPosition << std::endl; // 0, 1, 2, 3, 4, 5, 6, 7, 8
        std::cout << "Game Entry Block: " << gameEntryPosition / MAX_ENTRIES_PER_BLOCK << std::endl;
        std::cout << "Game Entry TEAM_ID_home: " << block.entries[gameEntryPosition % MAX_ENTRIES_PER_BLOCK].TEAM_ID_home << std::endl;
    }

    int startKey = 429;
    int endKey = 485;
    std::vector<int> gameEntryPositions = std::vector<int>();
    bptree.findRange(startKey, endKey, &gameEntryPositions);
    std::cout << "Game Entry Positions: ";
    for (int i = 0; i < gameEntryPositions.size(); i++)
    {
        std::cout << gameEntryPositions[i] << " ";
        GameEntryBlock block;
        dataFile.readGameEntryBlock(&block, gameEntryPositions[i]);
        std::cout << "Game Entry TEAM_ID_home: " << block.entries[0].TEAM_ID_home << std::endl;
        std::cout << "Game Entry FG_PCT_home: " << block.entries[0].FG_PCT_home << std::endl;
    }
    std::cout << std::endl;
}
