// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <ctime>

struct GameEntry // 33 bytes
{
    std::time_t GAME_DATE_EST; // 8 bytes
    int TEAM_ID_home;          // 4 bytes
    int PTS_home;              // 4 bytes
    float FG_PCT_home;         // Key // 4 bytes
    float FT_PCT_home;         // 4 bytes
    float FG3_PCT_home;        // 4 bytes
    int AST_home;              // 4 bytes
    int REB_home;              // 4 bytes
    bool HOME_TEAM_WINS;       // 1 byte
};

const int BLOCK_SIZE = 512;
const int ENTRY_SIZE = sizeof(GameEntry) + sizeof(int);
const int MAX_ENTRIES_PER_BLOCK = (BLOCK_SIZE - sizeof(int)) / ENTRY_SIZE;

struct GameEntryBlock
{
    int count; // Number of entries in the block
    GameEntry entries[MAX_ENTRIES_PER_BLOCK];
};

#endif // CONSTANTS_H