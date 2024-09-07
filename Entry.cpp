#include <iostream>
#include <chrono>

using namespace std;

class GameEntry
{
public:
    std::time_t GAME_DATE_EST; // 8 bytes
    int TEAM_ID_home;          // 4 bytes maybe need long
    int PTS_home;              // 4 bytes
    float FG_PCT_home;         // 4 bytes
    float FT_PCT_home;         // 4 bytes
    float FG3_PCT_home;        // 4 bytes
    int AST_home;              // 4 bytes
    int REB_home;              // 4 bytes
    bool HOME_TEAM_WINS;       // 1 byte
    // 33 bytes
};

main()
{
    GameEntry gameEntry;
    cout << "Size of GameEntry: " << sizeof(gameEntry) << " bytes" << endl; // 36 bytes (33 bytes + 3 bytes padding)
    return 0;
}