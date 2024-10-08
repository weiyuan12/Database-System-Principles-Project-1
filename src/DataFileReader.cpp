#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include "constants.h"

class DataFileReader
{
public:
    DataFileReader(const std::string &filePath) : file(filePath)
    {
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file.");
        }
        // Skip the header line
        std::getline(file, header);
    }

    ~DataFileReader()
    {
        if (file.is_open())
        {
            file.close();
        }
    }

    std::vector<GameEntry> readData()
    {
        std::vector<GameEntry> games;
        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            GameEntry game = {};
            std::string date;
            std::string wins;

            std::getline(ss, date, '\t');
            ss >> game.TEAM_ID_home;
            ss.ignore(); // ignore tab character
            ss >> game.PTS_home;
            ss.ignore(); // ignore tab character
            std::string fg_pct_home;
            std::getline(ss, fg_pct_home, '\t');
            game.FG_PCT_home = fg_pct_home.empty() ? std::numeric_limits<float>::quiet_NaN() : std::stod(fg_pct_home);
            ss.ignore(); // ignore tab character
            ss >> game.FT_PCT_home;
            ss.ignore(); // ignore tab character
            ss >> game.FG3_PCT_home;
            ss.ignore(); // ignore tab character
            ss >> game.AST_home;
            ss.ignore(); // ignore tab character
            ss >> game.REB_home;
            ss.ignore(); // ignore tab character
            ss >> wins;

            game.HOME_TEAM_WINS = (wins == "1");

            // Convert date string to std::time_t
            game.GAME_DATE_EST = convertDateToTimeT(date);

            games.push_back(game);
        }

        return games;
    }

private:
    std::ifstream file;
    std::string header;

    // Function to convert date string to std::time_t
    std::time_t convertDateToTimeT(const std::string &date)
    {
        std::tm tm = {};
        std::stringstream ss(date);
        ss >> std::get_time(&tm, "%d/%m/%Y");
        return std::mktime(&tm);
    }
};
