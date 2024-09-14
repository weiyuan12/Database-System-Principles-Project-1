#include <iostream>
#include <cstdio>
#include <cstring>
#include "DataFileReader.cpp"

int main()
{
    try
    {
        // DataFileReader reader("../data/game_short.txt");
        DataFileReader reader("../data/games.txt");
        std::vector<GameEntry> games = reader.readData();

        // Output the parsed data
        for (const auto &game : games)
        {
            std::cout << "Date: " << std::put_time(std::localtime(&game.GAME_DATE_EST), "%d/%m/%Y")
                      << ", Team ID Home: " << game.TEAM_ID_home
                      << ", Points Home: " << game.PTS_home
                      << ", FG% Home: " << game.FG_PCT_home
                      << ", FT% Home: " << game.FT_PCT_home
                      << ", FG3% Home: " << game.FG3_PCT_home
                      << ", AST Home: " << game.AST_home
                      << ", REB Home: " << game.REB_home
                      << ", Home Team Wins: " << (game.HOME_TEAM_WINS ? "Yes" : "No") << std::endl;
        }

        std::cout << "Number of games: " << games.size() << std::endl;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}