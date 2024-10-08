#include <iostream>
#include <fstream>
#include "../DataFile.cpp"
#include "../Storage.cpp"
#include "../constants.h"
#include <cassert>
#include <vector>

void testReadGameEntryBlock()
{
    std::fstream file("testReadGameEntryBlock", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
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

void testWriteAllGameEntryBlock()
{
    std::fstream file("testWriteAllGameEntryBlock", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    Storage storage(&file);
    DataFile dataFile(&storage);

    std::vector<GameEntry> gameEntries;

    for (int i = 0; i < MAX_ENTRIES_PER_BLOCK + 1; i++)
    {
        GameEntry gameEntry;
        gameEntry.GAME_DATE_EST = 1671840000 + i;
        gameEntry.TEAM_ID_home = 1610612000 + i;
        gameEntry.PTS_home = 100 + i;
        gameEntry.FG_PCT_home = 0.4 + i * 0.01;
        gameEntry.FT_PCT_home = 0.8 + i * 0.01;
        gameEntry.FG3_PCT_home = 0.3 + i * 0.01;
        gameEntry.AST_home = 20 + i;
        gameEntry.REB_home = 40 + i;
        gameEntry.HOME_TEAM_WINS = i % 2 == 0;

        gameEntries.push_back(gameEntry);
    }

    dataFile.writeAllGameEntries(gameEntries);

    GameEntryBlock *readBlock = new GameEntryBlock();
    dataFile.readGameEntryBlock(readBlock, 0);

    assert(readBlock->count == MAX_ENTRIES_PER_BLOCK);
    for (int i = 0; i < readBlock->count; i++)
    {
        assert(readBlock->entries[i].GAME_DATE_EST == gameEntries[i].GAME_DATE_EST);
        assert(readBlock->entries[i].TEAM_ID_home == gameEntries[i].TEAM_ID_home);
        assert(readBlock->entries[i].PTS_home == gameEntries[i].PTS_home);
        assert(readBlock->entries[i].FG_PCT_home == gameEntries[i].FG_PCT_home);
        assert(readBlock->entries[i].FT_PCT_home == gameEntries[i].FT_PCT_home);
        assert(readBlock->entries[i].FG3_PCT_home == gameEntries[i].FG3_PCT_home);
        assert(readBlock->entries[i].AST_home == gameEntries[i].AST_home);
        assert(readBlock->entries[i].REB_home == gameEntries[i].REB_home);
        assert(readBlock->entries[i].HOME_TEAM_WINS == gameEntries[i].HOME_TEAM_WINS);
    }

    std::cout << "first Block test passed" << std::endl;

    GameEntryBlock *readBlock2 = new GameEntryBlock();
    dataFile.readGameEntryBlock(readBlock2, 1);

    assert(readBlock2->count == 1);
    for (int i = 0; i < readBlock2->count; i++)
    {
        assert(readBlock2->entries[i].GAME_DATE_EST == gameEntries[MAX_ENTRIES_PER_BLOCK].GAME_DATE_EST);
        assert(readBlock2->entries[i].TEAM_ID_home == gameEntries[MAX_ENTRIES_PER_BLOCK].TEAM_ID_home);
        assert(readBlock2->entries[i].PTS_home == gameEntries[MAX_ENTRIES_PER_BLOCK].PTS_home);
        assert(readBlock2->entries[i].FG_PCT_home == gameEntries[MAX_ENTRIES_PER_BLOCK].FG_PCT_home);
        assert(readBlock2->entries[i].FT_PCT_home == gameEntries[MAX_ENTRIES_PER_BLOCK].FT_PCT_home);
        assert(readBlock2->entries[i].FG3_PCT_home == gameEntries[MAX_ENTRIES_PER_BLOCK].FG3_PCT_home);
        assert(readBlock2->entries[i].AST_home == gameEntries[MAX_ENTRIES_PER_BLOCK].AST_home);
        assert(readBlock2->entries[i].REB_home == gameEntries[MAX_ENTRIES_PER_BLOCK].REB_home);
        assert(readBlock2->entries[i].HOME_TEAM_WINS == gameEntries[MAX_ENTRIES_PER_BLOCK].HOME_TEAM_WINS);
    }

    std::cout << "second Block test passed" << std::endl;

    // std::remove("testWriteAllGameEntryBlock");
}

int main()
{
    testReadGameEntryBlock();
    // testWriteAllGameEntryBlock();

    return 0;
}