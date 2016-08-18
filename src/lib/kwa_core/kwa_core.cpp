
#include "kwa_core_pch.h"
#include "kwa_core.h"

bool kwa::loadMatchesFromCSVFile(MatchEstimator& estimator,
                                 const char* file_name)
{
    std::ifstream file(file_name);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::string token;

    struct TempMatch
    {
        std::string home_team;
        std::string guest_team;
        int date;
        int home_goals;
        int guest_goals;
    };
    std::vector<TempMatch> matches;
    std::getline(file, line);
    while (std::getline(file, line)) {
        TempMatch m;
        std::stringstream iss(line);

        if (!std::getline(iss, token, ',')) return false;

        if (!std::getline(iss, token, ',')) return false;
        {
            std::stringstream date_iss(token);
            std::getline(date_iss, token, '/');
            int day = std::stoi(token);
            std::getline(date_iss, token, '/');
            int month = std::stoi(token);
            std::getline(date_iss, token);
            int year = std::stoi(token);
            year += year > 18 ? 1900 : 2000;
            m.date = match_date(year, month, day);
        }

        if (!std::getline(iss, m.home_team, ',')) return false;

        if (!std::getline(iss, m.guest_team, ',')) return false;

        if (!std::getline(iss, token, ',')) return false;
        m.home_goals = std::stoi(token);

        if (!std::getline(iss, token, ',')) return false;
        m.guest_goals = std::stoi(token);

        matches.emplace_back(std::move(m));
    }

    for (auto& m : matches) {
        estimator.addMatch(m.date, m.home_team.c_str(), m.guest_team.c_str(),
                           m.home_goals, m.guest_goals);
    }

    estimator.recalculateTeamStatistics();
    return true;
}
