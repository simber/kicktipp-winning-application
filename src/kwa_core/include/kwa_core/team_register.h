#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "kwa_common.h"

namespace kwa {
/*!
 * @brief      Maps team names to unique ids.
 */
class TeamRegister
{
public:
    /*!
     * @brief      Add a new team and create a new id or return the id of an
     *             existing team.
     *
     * @param      name  (const std::string &): Name of team.
     *
     * @return     (TeamId): Valid team id.
     */
    auto registerTeam(const char* name) -> TeamId;

    /*!
     * @brief      Get the id of an existing team.
     *
     * @param      name  (const std::string &): Name of team.
     *
     * @return     (TeamId): Valid team id if team exists or INVALID_TEAM_ID if
     *             not.
     */
    auto getId(const char* name) const -> TeamId;

    /*!
     * @brief      Get the name of a team by its id.
     *
     * @param      id    (TeamId): Team id MUST be valid.
     *
     * @return     (const std::string&): Team name.
     */
    auto teamName(TeamId id) const -> const std::string&;

    /*!
     * @brief      Deletes all registered teams.
     */
    void clear();

    /*!
     * @brief      Returns the current count of teams that have been registered.
     *
     * @return     (size_t): Count of teams.
     */
    auto size() const { return m_team_names.size(); }

private:
    std::vector<std::string> m_team_names;
    std::unordered_map<std::string, TeamId> m_team_ids_by_name;
};
} // namespace kwa
