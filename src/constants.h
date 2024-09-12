// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

const int BLOCK_SIZE = 512;
const int ENTRY_SIZE = sizeof(GameEntry) + sizeof(int); // 33 bytes
const int maxEntriesPerBlock = (BLOCK_SIZE - sizeof(int) - sizeof(bool)) / (ENTRY_SIZE + sizeof(int));
// 512 - 4 - 1 = 507
// 507 / 33 = 15

struct GameEntry // 29 bytes
{
    std::time_t GAME_DATE_EST;
    int TEAM_ID_home;    // 4 bytes
    int PTS_home;        // 4 bytes
    float FG_PCT_home;   // 4 bytes
    float FT_PCT_home;   // 4 bytes
    float FG3_PCT_home;  // 4 bytes
    int AST_home;        // 4 bytes
    int REB_home;        // 4 bytes
    bool HOME_TEAM_WINS; // 1 byte
};

struct Node
{
    int n;
    GameEntry entries[maxEntriesPerBlock];
    Node *children[maxEntriesPerBlock + 1];
    bool leaf;
};

#endif // CONSTANTS_H