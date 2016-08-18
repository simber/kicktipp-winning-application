#pragma once

#include "stats_provider.h"
#include "team_register.h"

namespace kwa
{
struct MatchEstimation
{
    ThreeWayBet three_way_probabilities;

    int most_likely_home_goals;
    int most_likely_guest_goals;
    float most_likely_result_probability;

    int best_result_bet_home_goals;
    int best_result_bet_guest_goals;
    float best_result_bet_ev;
    float best_result_bet_probability;
};

class MatchEstimator
{
public:
    /**
     * @brief      Default constructor. Add matches via addMatch() before using
     *             estimate()
     */
    MatchEstimator()
        : m_team_statistics{0}, m_system_points{4.0f, 3.0f, 2.0f},
          m_max_date(-1)
    {
    }

    /**
     * @brief      Sets the maximum date. Only matches on dates before it will
     *             be used for estimations.
     *
     * @param[in]  date  The date.
     */
    void setMaxDate(int date) { m_max_date = date; }

    /**
     * @brief      Returns the number of added matches.
     *
     * @return     match count
     */
    size_t matchCount() const { return m_matches.size(); }

    /**
     * @brief      Clears all added matches.
     */
    void clear();

    /**
     * @brief      Adds a match to the database. recalculateTeamStatistics()
     *             MUST BE called after new matches have been added to update
     *             statitics.
     *
     * @param[in]  date         The date of the match.
     * @param[in]  home_team    The name of the home team.
     * @param[in]  guest_team   The name of the guest team.
     * @param[in]  home_goals   The amount of goals scored by the home team
     * @param[in]  guest_goals  The amount of goals scored by the guest team.
     */
    void addMatch(int date, const char* home_team, const char* guest_team,
                  int home_goals, int guest_goals);

    /**
     * @brief      HAS TO be called after new matches have been added.
     */
    void recalculateTeamStatistics();

    /**
     * @brief      Checks if team has statistics for home matches. If max date
     *             is set this method will only check if the has statistics
     *             BEFORE the max date.
     *
     * @param[in]  team_name  Team name.
     *
     * @return     true if data exists.
     */
    bool hasHomeStatistics(const char* team_name) const;

    /**
     * @brief      Checks if team has statistics for away matches. If max date
     *             is set this method will only check if the has statistics
     *             BEFORE the max date.
     *
     * @param[in]  team_name  Team name.
     *
     * @return     true if data exists.
     */
    bool hasGuestStatistics(const char* team_name) const;

    /**
     * @brief      Estimates the match. There MUST BE statistics for both teams.
     *
     * @param      out         output
     * @param[in]  home_team   Home team name.
     * @param[in]  guest_team  Guest team name.
     */
    void estimate(MatchEstimation& out, const char* home_team,
                  const char* guest_team);

    /**
     * @brief      Estimates the match. There MUST BE statistics for both teams.
     *
     * @param      out         output
     * @param[in]  home_team   Home team id.
     * @param[in]  guest_team  Guest team id.
     */
    void estimate(MatchEstimation& out, TeamId home_team, TeamId guest_team);

    const TeamRegister& teamRegister() const { return m_team_register; }
    const gsl::span<const MatchData> matches() const { return m_matches; }

private:
    std::vector<MatchData> m_matches;
    TeamRegister m_team_register;
    StatsProvider m_team_statistics;
    kwa::BetSystemPoints m_system_points;
    int m_max_date;

    void getStatistics(TeamId home_id, TeamId guest_id, TeamStats& home_stats,
                       TeamStats& guest_stats, LeagueStats& league_stats) const;
};
}
