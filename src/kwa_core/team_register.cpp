#include "kwa_common.h"
#include "team_register.h"

auto kwa::TeamRegister::registerTeam(const char* name) -> TeamId
{
    auto find_it = m_team_ids_by_name.find(name);
    if (find_it != m_team_ids_by_name.end()) return find_it->second;

    TeamId id = static_cast<TeamId>(m_team_names.size());
    m_team_names.push_back(name);
    m_team_ids_by_name.emplace(name, id);
    return id;
}

auto kwa::TeamRegister::getId(const char* name) const -> TeamId
{
    auto find_it = m_team_ids_by_name.find(name);
    if (find_it == m_team_ids_by_name.end()) return INVALID_TEAM_ID;

    return find_it->second;
}

auto kwa::TeamRegister::teamName(TeamId id) const -> const std::string &
{
    assert(id != INVALID_TEAM_ID);
    assert(id < (int)m_team_names.size());
    return m_team_names[(size_t)id];
}

void kwa::TeamRegister::clear()
{
    m_team_ids_by_name.clear();
    m_team_names.clear();
}
