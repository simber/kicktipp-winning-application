
#include "kwa_core_pch.h"
#include "kwa_core.h"

void kwa::loadMatchesFromCSVFile(MatchEstimator& estimator,
                                 const char* file_name)
{
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cout << "error loading matches." << std::endl;
    }

    std::string line;
    std::string token;
    std::string home_team;
    std::string guest_team;

    std::getline(file, line);
    while (std::getline(file, line)) {
        kwa::MatchData m;
        std::stringstream iss(line);

        if (!std::getline(iss, token, ',')) assert(0 == "error");

        if (!std::getline(iss, token, ',')) assert(0 == "error");
        {
            std::stringstream date_iss(token);
            std::getline(date_iss, token, '/');
            int day = std::stoi(token);
            std::getline(date_iss, token, '/');
            int month = std::stoi(token);
            std::getline(date_iss, token);
            int year = std::stoi(token);
            year += year > 18 ? 1900 : 2000;
            m.day = match_date(year, month, day);
        }

        if (!std::getline(iss, home_team, ',')) assert(0 == "error");

        if (!std::getline(iss, guest_team, ',')) assert(0 == "error");

        if (!std::getline(iss, token, ',')) assert(0 == "error");
        m.home_goals = std::stoi(token);

        if (!std::getline(iss, token, ',')) assert(0 == "error");
        m.guest_goals = std::stoi(token);

        estimator.addMatch(m.day, home_team.c_str(), guest_team.c_str(),
                           m.home_goals, m.guest_goals);
    }
    estimator.recalculateTeamStatistics();
}
