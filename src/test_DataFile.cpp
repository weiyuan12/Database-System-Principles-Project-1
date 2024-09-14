#include <iostream>
#include <fstream>
#include "DataFile.cpp"
#include "Storage.cpp"
#include "constants.h"
#include <cassert>

void testReadGameEntryBlock()
{
    std::fstream file("testReadGameEntryBlock", std::ios::in | std::ios::out | std::ios::trunc);
    Storage storage(&file);
    DataFile dataFile(&storage);

    // Create a dummy GameEntryBlock and write it to the file
    GameEntryBlock writeBlock;
    writeBlock.count = 5;

    // GAME_DATE_EST, TEAM_ID_home, PTS_home, FG_PCT_home, FT_PCT_home, FG3_PCT_home, AST_home, REB_home, HOME_TEAM_WINS
    // 22/12/2022, 1610612740, 126, 0.484, 0.926, 0.382, 25, 46, 1
    for (int i = 0; i < writeBlock.count; i++)
    {
        writeBlock.entries[i].GAME_DATE_EST = 1671840000 + i;
        writeBlock.entries[i].TEAM_ID_home = 1610612000 + i;
        writeBlock.entries[i].PTS_home = 100 + i;
        writeBlock.entries[i].FG_PCT_home = 0.4 + i * 0.01;
        writeBlock.entries[i].FT_PCT_home = 0.8 + i * 0.01;
        writeBlock.entries[i].FG3_PCT_home = 0.3 + i * 0.01;
        writeBlock.entries[i].AST_home = 20 + i;
        writeBlock.entries[i].REB_home = 40 + i;
        writeBlock.entries[i].HOME_TEAM_WINS = i % 2 == 0;
    }

    // Initialize writeBlock with test data
    dataFile.addGameEntryBlock(&writeBlock);

    // Read the GameEntryBlock from the file
    // GameEntryBlock *readBlock = dataFile.readGameEntryBlock(0);

    GameEntryBlock *readBlock = new GameEntryBlock();
    dataFile.readGameEntryBlock(readBlock, 0);

    assert(readBlock->count == writeBlock.count);
    for (int i = 0; i < writeBlock.count; i++)
    {
        assert(readBlock->entries[i].GAME_DATE_EST == writeBlock.entries[i].GAME_DATE_EST);
        assert(readBlock->entries[i].TEAM_ID_home == writeBlock.entries[i].TEAM_ID_home);
        assert(readBlock->entries[i].PTS_home == writeBlock.entries[i].PTS_home);
        assert(readBlock->entries[i].FG_PCT_home == writeBlock.entries[i].FG_PCT_home);
        assert(readBlock->entries[i].FT_PCT_home == writeBlock.entries[i].FT_PCT_home);
        assert(readBlock->entries[i].FG3_PCT_home == writeBlock.entries[i].FG3_PCT_home);
        assert(readBlock->entries[i].AST_home == writeBlock.entries[i].AST_home);
        assert(readBlock->entries[i].REB_home == writeBlock.entries[i].REB_home);
        assert(readBlock->entries[i].HOME_TEAM_WINS == writeBlock.entries[i].HOME_TEAM_WINS);
    }

    std::cout << "testReadGameEntryBlock passed" << std::endl;

    std::remove("testReadGameEntryBlock");
}

int main()
{
    testReadGameEntryBlock();

    return 0;
}