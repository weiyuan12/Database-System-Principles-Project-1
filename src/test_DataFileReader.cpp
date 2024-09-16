#include <iostream>
#include <cstdio>
#include <cstring>
#include "DataFileReader.cpp"
#include "constants.h"
#include "DataFile.cpp"
#include "Storage.cpp"
#include <cassert>

int main()
{
    DataFileReader reader("../data/game_short.txt");
    // DataFileReader reader("../data/games.txt");
    std::vector<GameEntry> games = reader.readData();

    // Output the parsed data
    // for (const auto &game : games)
    // {
    //     std::cout << "Date: " << std::put_time(std::localtime(&game.GAME_DATE_EST), "%d/%m/%Y")
    //               << ", Team ID Home: " << game.TEAM_ID_home
    //               << ", Points Home: " << game.PTS_home
    //               << ", FG% Home: " << game.FG_PCT_home
    //               << ", FT% Home: " << game.FT_PCT_home
    //               << ", FG3% Home: " << game.FG3_PCT_home
    //               << ", AST Home: " << game.AST_home
    //               << ", REB Home: " << game.REB_home
    //               << ", Home Team Wins: " << (game.HOME_TEAM_WINS ? "Yes" : "No") << std::endl;
    // }

    std::cout << "Number of games: " << games.size() << std::endl;

    // GameEntryBlock writeBlock;
    // writeBlock.count = games.size();

    std::vector<GameEntryBlock> blocks;

    size_t index = 0;
    while (index < games.size())
    {
        GameEntryBlock block;
        block.count = 0;

        // Copy entries to the current block
        while (block.count < MAX_ENTRIES_PER_BLOCK && index < games.size())
        {
            block.entries[block.count] = games[index];
            ++block.count;
            ++index;
        }

        blocks.push_back(block);
    }

    for (size_t blockIndex = 0; blockIndex < blocks.size(); ++blockIndex)
    {
        std::cout << "Block " << blockIndex << ":" << std::endl;
        const GameEntryBlock &block = blocks[blockIndex];

        for (int i = 0; i < block.count; ++i)
        {
            const GameEntry &game = block.entries[i];
            // std::cout << "Date: " << std::put_time(std::localtime(&game.GAME_DATE_EST), "%d/%m/%Y")
            //           << ", Team ID Home: " << game.TEAM_ID_home
            //           << ", Points Home: " << game.PTS_home
            //           << ", FG% Home: " << game.FG_PCT_home
            //           << ", FT% Home: " << game.FT_PCT_home
            //           << ", FG3% Home: " << game.FG3_PCT_home
            //           << ", AST Home: " << game.AST_home
            //           << ", REB Home: " << game.REB_home
            //           << ", Home Team Wins: " << (game.HOME_TEAM_WINS ? "Yes" : "No") << std::endl;
        }
    }
    std::cout << "Number of games: " << games.size() << std::endl;

    std::fstream file("testReadGameEntryBlock", std::ios::in | std::ios::out | std::ios::trunc);
    Storage storage(&file);
    DataFile dataFile(&storage);

    // Add blocks to file
    for (auto &block : blocks) 
    {
        dataFile.addGameEntryBlock(&block);
    }

    //dataFile.addGameEntryBlock(&writeBlock);
    GameEntryBlock *readBlock = new GameEntryBlock();

    std::cout << "Number of blocks: " << blocks.size() << std::endl;

    // TODO
    // Read the blocks back and verify
    for (size_t blockIndex = 0; blockIndex < blocks.size(); ++blockIndex)
    {
        std::cout << "Processing block index: " << blockIndex << std::endl;

        GameEntryBlock readBlock;
        dataFile.readGameEntryBlock(&readBlock, blockIndex);

        std::cout << "Block read successfully: " << blockIndex << std::endl;
        std::cout << "Read Block size: " << readBlock.count << std::endl;

        const GameEntryBlock &expectedBlock = blocks[blockIndex];
        std::cout << "Expected Block size: " << expectedBlock.count << std::endl;

        std::cout << "Comparing blocks..." << std::endl;

        // Verify count
        std::cout << "Expected count: " << expectedBlock.count << ", Read count: " << readBlock.count << std::endl;
        assert(readBlock.count == expectedBlock.count);

        for (int i = 0; i < expectedBlock.count; i++)
        {
            std::cout << "Comparing entry " << i << ":" << std::endl;

            const GameEntry &readEntry = readBlock.entries[i];
            const GameEntry &expectedEntry = expectedBlock.entries[i];

            std::cout << "  Expected Game Date: " << std::put_time(std::localtime(&expectedEntry.GAME_DATE_EST), "%d/%m/%Y")
                      << ", Read Game Date: " << std::put_time(std::localtime(&readEntry.GAME_DATE_EST), "%d/%m/%Y") << std::endl;

            std::cout << "  Expected Team ID Home: " << expectedEntry.TEAM_ID_home
                      << ", Read Team ID Home: " << readEntry.TEAM_ID_home << std::endl;

            std::cout << "  Expected Points Home: " << expectedEntry.PTS_home
                      << ", Read Points Home: " << readEntry.PTS_home << std::endl;

            std::cout << "  Expected FG% Home: " << expectedEntry.FG_PCT_home
                      << ", Read FG% Home: " << readEntry.FG_PCT_home << std::endl;

            std::cout << "  Expected FT% Home: " << expectedEntry.FT_PCT_home
                      << ", Read FT% Home: " << readEntry.FT_PCT_home << std::endl;

            std::cout << "  Expected FG3% Home: " << expectedEntry.FG3_PCT_home
                      << ", Read FG3% Home: " << readEntry.FG3_PCT_home << std::endl;

            std::cout << "  Expected AST Home: " << expectedEntry.AST_home
                      << ", Read AST Home: " << readEntry.AST_home << std::endl;

            std::cout << "  Expected REB Home: " << expectedEntry.REB_home
                      << ", Read REB Home: " << readEntry.REB_home << std::endl;

            std::cout << "  Expected Home Team Wins: " << (expectedEntry.HOME_TEAM_WINS ? "Yes" : "No")
                      << ", Read Home Team Wins: " << (readEntry.HOME_TEAM_WINS ? "Yes" : "No") << std::endl;

            assert(readEntry.GAME_DATE_EST == expectedEntry.GAME_DATE_EST);
            assert(readEntry.TEAM_ID_home == expectedEntry.TEAM_ID_home);
            assert(readEntry.PTS_home == expectedEntry.PTS_home);
            assert(readEntry.FG_PCT_home == expectedEntry.FG_PCT_home);
            // assert(readEntry.FT_PCT_home == expectedEntry.FT_PCT_home);
            // assert(readEntry.AST_home == expectedEntry.AST_home);
            // assert(readEntry.REB_home == expectedEntry.REB_home);
            // assert(readEntry.HOME_TEAM_WINS == expectedEntry.HOME_TEAM_WINS);
        }

        std::cout << "Block " << blockIndex << " verification passed." << std::endl;
    }

    std::cout << "testReadGameEntryBlock passed" << std::endl;
    std::remove("testReadGameEntryBlock");

    return 0;
}