// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <ctime>
#include <bitset>

struct GameEntry // 40 bytes
{
    std::time_t GAME_DATE_EST; // 8 bytes // seems to become 4 bytes when running
    int TEAM_ID_home;          // 4 bytes
    int PTS_home;              // 4 bytes
    float FG_PCT_home;         // Key // 4 bytes
    float FT_PCT_home;         // 4 bytes
    float FG3_PCT_home;        // 4 bytes
    int AST_home;              // 4 bytes
    int REB_home;              // 4 bytes
    int HOME_TEAM_WINS;        // 1 byte
};

const int BLOCK_SIZE = 512;
const int ENTRY_SIZE = sizeof(GameEntry);
const int MAX_ENTRIES_PER_BLOCK = (BLOCK_SIZE - sizeof(int)) / ENTRY_SIZE;
const int MAX_INDEX_PER_BLOCK = (BLOCK_SIZE - sizeof(int) - sizeof(int)) / (sizeof(int) + sizeof(int));

const int GameEntryBlockPadding = BLOCK_SIZE - sizeof(int) - MAX_ENTRIES_PER_BLOCK * ENTRY_SIZE;
struct GameEntryBlock
{
    GameEntry entries[MAX_ENTRIES_PER_BLOCK];
    int count; // Number of entries in the block

private:
    char padding[GameEntryBlockPadding] = {"a"};
};

const int t = sizeof(GameEntryBlock);

const int IndexBlockPadding = BLOCK_SIZE - sizeof(int) - MAX_INDEX_PER_BLOCK * sizeof(int) - (MAX_INDEX_PER_BLOCK + 1) * sizeof(int);
struct IndexBlock
{
    int keys[MAX_INDEX_PER_BLOCK];
    int childrenPtr[MAX_INDEX_PER_BLOCK + 1];
    int count; // Number of entries in the block
private:
    char padding[IndexBlockPadding];
};

static_assert(sizeof(GameEntryBlock) == BLOCK_SIZE, "GameEntryBlock size is not equal to BLOCK_SIZE");
static_assert(sizeof(IndexBlock) == BLOCK_SIZE, "IndexBlock size is not equal to BLOCK_SIZE");

#endif // CONSTANTS_H