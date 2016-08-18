#pragma once

#include <vector>
#include "kwa_common.h"

namespace kwa
{
/**
 * @brief      Stores match data for individual teams and provides statistics
 *             like avg. scored goals separated for home and away matches. Used
 *             by MatchEstimator to give estimations based on former match
 *             results.
 */
class StatsProvider
{
public:
    /**
     * @brief      Default constructor, creates an empty StatsProvider. Use
     *             addMatches() to add data.
     */
    StatsProvider() {}

    /**
     * @brief      Constructor, registers a specified amount of teams with empty
     *             data. To avoid resizing while adding match data.
     *
     * @param[in]  team_count  The number of teams.
     */
    explicit StatsProvider(size_t team_count) : m_team_data(team_count) {}

    /**
     * @brief      Registers a specified amount of teams with empty data. To
     *             avoid resizing while adding match data.
     *
     * @param[in]  team_count  THe number of teams.
     */
    void setTeamCount(size_t team_count);

    /**
     * @brief      Clears all added match data and deletes all registered teams.
     */
    void clear();

    /**
     * @brief      Adds a range of matches. The range MUST be sorted by date in
     *             ascending order. Automatically registers teams with an id
     *             greater than the specified team_count in the constructor or
     *             via setTeamCount().
     *
     * @param[in]  matches   The range of matches. SORTED by date.
     * @param[in]  goal_cap  An optional goal cap. Example: goal_cap=5, then a
     *                       9:1 will be interpreted as 5:1 and a 6:5 as 5:5.
     */
    void addMatches(gsl::span<const MatchData> matches, int goal_cap = 0);

    /**
     * @brief      Returns the total number of matches that have been added.
     *
     * @return     match count.
     */
    size_t matchCount() const;

    /**
     * @brief      Returns the number of teams that have been registered.
     *
     * @return     registered teams.
     */
    size_t teamCount() const { return m_team_data.size(); }

    /**
     * @brief      Checks if there is data of home matches for a team.
     *
     * @param[in]  team      team id
     * @param[in]  max_date  Optional date. If set it will be checked if there
     *                       is data before the specified date for the team. If
     *                       set to -1 it will be ignored.
     *
     * @return     true if data available
     */
    bool hasHomeStats(TeamId team, int max_date = -1) const;

    /**
     * @brief      Checks if there is data of home matches for a team.
     *
     * @param[in]  team      team id
     * @param[in]  max_date  Optional date. If set it will be checked if there
     *                       is data before the specified date for the team. If
     *                       set to -1 it will be ignored.
     *
     * @return     true if data available
     */
    bool hasGuestStats(TeamId team, int max_date = -1) const;

    /**
     * @brief      Gets the home stats from one team. Including all matches that
     *             have been added.
     *
     * @param[in]  team  Team id. The team MUST be in the database and MUST have
     *                   match data.
     * @param      out   Output.
     *
     * @return     the number of matches considered.
     */
    int getHomeStats(TeamId team, TeamStats& out) const;

    /**
     * @brief      Gets the guest stats from one team. Including all matches
     *             that have been added.
     *
     * @param[in]  team  Team id. The team MUST be in the database and MUST
     *                   have match data.
     * @param      out   Output.
     *
     * @return     the number of matches considered.
     */
    int getGuestStats(TeamId team, TeamStats& out) const;

    /**
     * @brief      Gets the home stats from one team. Including only a maximal
     *             number of recent matches. that have been added.
     *
     * @param[in]  team         Team id. The team MUST be in the database and
     *                          MUST have match data.
     * @param[in]  num_matches  Number of matches to consider.
     * @param      out          Output.
     *
     * @return     the number of matches actually considered.
     */
    int getHomeStats(TeamId team, size_t num_matches, TeamStats& out) const;

    /**
     * @brief      Gets the guest stats from one team. Including only a maximal
     *             number of recent matches. that have been added.
     *
     * @param[in]  team         Team id. The team MUST be in the database and
     *                          MUST have match data.
     * @param[in]  num_matches  Number of matches to consider.
     * @param      out          Output.
     *
     * @return     the number of matches actually considered.
     */
    int getGuestStats(TeamId team, size_t num_matches, TeamStats& out) const;

    /**
     * @brief      Gets the home stats from one team. Including only matches
     *             before the specified date.
     *
     * @param[in]  team         Team id. The team MUST be in the database and
     *                          MUST have match data.
     * @param[in]  before_date  The date.
     * @param      out          Output.
     *
     * @return     the number of matches actually considered.
     */
    int getHomeStatsBefore(TeamId team, int before_date, TeamStats& out) const;

    /**
     * @brief      Gets the guest stats from one team. Including only matches
     *             before the specified date.
     *
     * @param[in]  team         Team id. The team MUST be in the database and
     *                          MUST have match data.
     * @param[in]  before_date  The date.
     * @param      out          Output.
     *
     * @return     the number of matches actually considered.
     */
    int getGuestStatsBefore(TeamId team, int before_date, TeamStats& out) const;

    /**
     * @brief      Gets the home stats from one team. Including only matches
     *             before the specified date and only a maximum number of
     *             matches.
     *
     * @param[in]  team         Team id. The team MUST be in the database and
     *                          MUST have match data.
     * @param[in]  before_date  The date.
     * @param[in]  num_matches  Maximum number of matches.
     * @param      out          Output.
     *
     * @return     the number of matches actually considered.
     */
    int getHomeStatsBefore(TeamId team, int before_date, size_t num_matches,
                           TeamStats& out) const;
    /**
     * @brief      Gets the guest stats from one team. Including only matches
     *             before the specified date and only a maximum number of
     *             matches.
     *
     * @param[in]  team         Team id. The team MUST be in the database and
     *                          MUST have match data.
     * @param[in]  before_date  The date.
     * @param[in]  num_matches  Maximum number of matches.
     * @param      out          Output.
     *
     * @return     the number of matches actually considered.
     */
    int getGuestStatsBefore(TeamId team, int before_date, size_t num_matches,
                            TeamStats& out) const;

    /**
     * @brief      Gets the overall stats of all added matches.
     *
     * @param      out   Output.
     *
     * @return     The number of matches considered.
     */
    int getLeagueStats(LeagueStats& out) const;

    /**
     * @brief      Gets the overall stats. Considering a fixed amount of
     *             matches, starting with the most recent ones.
     *
     * @param[in]  num_matches  Maximum number of matches.
     * @param      out          Output
     *
     * @return     The number of matches actually considered.
     */
    int getLeagueStats(size_t num_matches, LeagueStats& out) const;

    /**
     * @brief      Gets the overall stats. Considering every match before the
     *             specified date.
     *
     * @param[in]  before_date  The date.
     * @param      out          Output.
     *
     * @return     The number of matches considered.
     */
    int getLeagueStatsBefore(int before_date, LeagueStats& out) const;

    /**
     * @brief      Gets the overall stats. Considering only matches before the
     *             specified date and only a maximum number of matches.
     *
     * @param[in]  before_date  The date.
     * @param[in]  num_matches  Maximum number of matches.
     * @param      out          Output.
     *
     * @return     The number of matches actually considered.
     */
    int getLeagueStatsBefore(int before_date, size_t num_matches,
                             LeagueStats& out) const;

private:
    struct PerTeamData
    {
        std::vector<int> home_goals;
        std::vector<int> home_against;
        std::vector<int> away_goals;
        std::vector<int> away_against;
        std::vector<int> home_dates;
        std::vector<int> guest_dates;
    };
    std::vector<PerTeamData> m_team_data;
    PerTeamData m_overall_stats;

    void registerTeam(TeamId team_id);
    PerTeamData& getTeamData(TeamId team_id);
    const PerTeamData& getTeamData(TeamId team_id) const;
};
}
