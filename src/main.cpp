#include "./BPTree.cpp"
#include "./DataFile.cpp"
#include "./constants.h"
#include "./DataFileReader.cpp"
#include <fstream>
#include <iostream>

enum class UserAction
{
    BUILD_DB,
    QUERY_DB
};

UserAction getUserAction()
{
    int choice;
    std::cout << "Select an action:\n";
    std::cout << "1. Build Database\n";
    std::cout << "2. Query Database\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;

    switch (choice)
    {
    case 1:
        return UserAction::BUILD_DB;
    case 2:
        return UserAction::QUERY_DB;
    default:
        std::cerr << "Invalid choice, defaulting to Query Database." << std::endl;
        return UserAction::QUERY_DB;
    }
}

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

void makeRangeQuery()
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

    int startKey, endKey;
    std::cout << "Enter the start key: ";
    std::cin >> startKey;
    std::cout << "Enter the end key: ";
    std::cin >> endKey;

    std::vector<int> result;
    bptree.findRange(startKey, endKey, result);

    for (int i = 0; i < result.size(); i++)
    {
        GameEntryBlock block;
        dataFile.readGameEntryBlock(&block, result[i]);
        for (int j = 0; j < block.count; j++)
        {
            std::cout << block.entries[j].AST_home
                      << " " << block.entries[j].FG_PCT_home
                      << " " << block.entries[j].FT_PCT_home
                      << " " << block.entries[j].FG3_PCT_home
                      << " " << block.entries[j].REB_home
                      << " " << block.entries[j].PTS_home
                      << " " << block.entries[j].TEAM_ID_home
                      << " " << block.entries[j].HOME_TEAM_WINS
                      << std::endl;
        }
    }

    std::cout << "Total data blocks read: " << entriesStorage.getReadCount() << std::endl;
    std::cout << "Total index blocks read: " << indexStorage.getReadCount() << std::endl;
}

int main()
{
    while (true)
    {
        UserAction action = getUserAction();

        switch (action)
        {
        case UserAction::BUILD_DB:
            buildDB();
            break;
        case UserAction::QUERY_DB:
            makeRangeQuery();
            break;
        default:
            std::cerr << "Invalid action." << std::endl;
            return 1;
        }
    }
}
