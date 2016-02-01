#include "kwa_core_pch.h"
#include "stats_provider.h"

void kwa::StatsProvider::clear() { m_team_data.clear(); }
void kwa::StatsProvider::setTeamCount(size_t team_count)
{
    m_team_data.resize(team_count);
}

size_t kwa::StatsProvider::matchCount() const
{
    return m_overall_stats.home_goals.size();
}

void kwa::StatsProvider::addMatches(gsl::span<const MatchData> matches,
                                    int goal_cap)
{
    auto& overall = m_overall_stats;

    for (auto& m : matches) {
        registerTeam(m.home_team);
        registerTeam(m.guest_team);
        auto& home = getTeamData(m.home_team);
        auto& guest = getTeamData(m.guest_team);

        auto add_summed = [](auto& vec, auto value) {
            vec.push_back(vec.size() > 0 ? vec.back() + value : value);
        };

        int home_goals =
            goal_cap > 0 && m.home_goals > goal_cap ? goal_cap : m.home_goals;
        int guest_goals =
            goal_cap > 0 && m.guest_goals > goal_cap ? goal_cap : m.guest_goals;

        add_summed(home.home_goals, home_goals);
        add_summed(home.home_against, guest_goals);
        home.home_dates.push_back(m.day);

        add_summed(guest.away_goals, guest_goals);
        add_summed(guest.away_against, home_goals);
        guest.guest_dates.push_back(m.day);

        add_summed(overall.home_goals, home_goals);
        add_summed(overall.home_against, guest_goals);
        overall.home_dates.push_back(m.day);
    }
}

bool kwa::StatsProvider::hasHomeStats(TeamId team, int max_date) const
{
    if ((size_t)team >= m_team_data.size()) return false;
    auto& team_data = getTeamData(team);
    if (max_date < 0) return team_data.home_goals.size() > 0;
    auto it = std::lower_bound(team_data.home_dates.begin(),
                               team_data.home_dates.end(), max_date);

    return (it != team_data.home_dates.begin());
}

bool kwa::StatsProvider::hasGuestStats(TeamId team, int max_date) const
{
    if ((size_t)team >= m_team_data.size()) return false;
    auto& team_data = getTeamData(team);
    if (max_date < 0) return team_data.away_goals.size() > 0;
    auto it = std::lower_bound(team_data.guest_dates.begin(),
                               team_data.guest_dates.end(), max_date);

    return (it != team_data.guest_dates.begin());
}

int kwa::StatsProvider::getHomeStats(TeamId team, TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    int count = (int)team_data.home_goals.size();
    assert(count > 0);

    int sum_goals = team_data.home_goals.back();
    int sum_against = team_data.home_against.back();

    out.goal_avg[TeamStats::HOME] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::HOME] = (float)sum_against / (float)count;
    out.match_count[TeamStats::HOME] = (float)count;

    return count;
}

int kwa::StatsProvider::getGuestStats(TeamId team, TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    int count = (int)team_data.away_goals.size();
    assert(count > 0);

    int sum_goals = team_data.away_goals.back();
    int sum_against = team_data.away_against.back();

    out.goal_avg[TeamStats::AWAY] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::AWAY] = (float)sum_against / (float)count;
    out.match_count[TeamStats::AWAY] = (float)count;

    return count;
}

int kwa::StatsProvider::getHomeStats(TeamId team, size_t num_matches,
                                     TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    assert(team_data.home_goals.size() > 0);
    size_t last_match = team_data.home_goals.size() - 1;

    int sum_goals = num_matches > last_match
                        ? team_data.home_goals[last_match]
                        : team_data.home_goals[last_match] -
                              team_data.home_goals[last_match - num_matches];
    int sum_against =
        num_matches > last_match
            ? team_data.home_against[last_match]
            : team_data.home_against[last_match] -
                  team_data.home_against[last_match - num_matches];

    size_t count = num_matches > last_match ? last_match + 1 : num_matches;
    out.goal_avg[TeamStats::HOME] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::HOME] = (float)sum_against / (float)count;
    out.match_count[TeamStats::HOME] = (float)count;

    return (int)count;
}

int kwa::StatsProvider::getGuestStats(TeamId team, size_t num_matches,
                                      TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    assert(team_data.home_goals.size() > 0);
    size_t last_match = team_data.away_goals.size() - 1;

    int sum_goals = num_matches > last_match
                        ? team_data.away_goals[last_match]
                        : team_data.away_goals[last_match] -
                              team_data.away_goals[last_match - num_matches];
    int sum_against =
        num_matches > last_match
            ? team_data.away_against[last_match]
            : team_data.away_against[last_match] -
                  team_data.away_against[last_match - num_matches];

    size_t count = num_matches > last_match ? last_match + 1 : num_matches;
    out.goal_avg[TeamStats::AWAY] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::AWAY] = (float)sum_against / (float)count;
    out.match_count[TeamStats::AWAY] = (float)count;

    return (int)count;
}

int kwa::StatsProvider::getLeagueStats(LeagueStats& out) const
{
    auto& team_data = m_overall_stats;
    int count = (int)team_data.home_goals.size();
    assert(count >= 0);

    int sum_goals = team_data.home_goals.back();
    int sum_against = team_data.home_against.back();

    out.goal_avg[LeagueStats::HOME] = (float)sum_goals / (float)count;
    out.goal_avg[LeagueStats::AWAY] = (float)sum_against / (float)count;
    out.goal_avg[LeagueStats::TOTAL] = 0.5f * (out.goal_avg[LeagueStats::HOME] +
                                               out.goal_avg[LeagueStats::AWAY]);
    out.match_count = (float)count;

    return count;
}

int kwa::StatsProvider::getLeagueStats(size_t num_matches,
                                       LeagueStats& out) const
{
    auto& team_data = m_overall_stats;
    assert(team_data.home_goals.size() > 0);
    size_t last_match = team_data.home_goals.size() - 1;

    int sum_goals = num_matches > last_match
                        ? team_data.home_goals[last_match]
                        : team_data.home_goals[last_match] -
                              team_data.home_goals[last_match - num_matches];
    int sum_against =
        num_matches > last_match
            ? team_data.home_against[last_match]
            : team_data.home_against[last_match] -
                  team_data.home_against[last_match - num_matches];

    size_t count = num_matches > last_match ? last_match + 1 : num_matches;
    out.goal_avg[LeagueStats::HOME] = (float)sum_goals / (float)count;
    out.goal_avg[LeagueStats::AWAY] = (float)sum_against / (float)count;
    out.goal_avg[LeagueStats::TOTAL] = 0.5f * (out.goal_avg[LeagueStats::HOME] +
                                               out.goal_avg[LeagueStats::AWAY]);
    out.match_count = (float)count;

    return (int)count;
}

int kwa::StatsProvider::getLeagueStatsBefore(int before_date,
                                             size_t num_matches,
                                             LeagueStats& out) const
{
    auto& team_data = m_overall_stats;
    assert(team_data.home_goals.size() > 0);
    auto it = std::lower_bound(team_data.home_dates.begin(),
                               team_data.home_dates.end(), before_date);

    auto distance = std::distance(team_data.home_dates.begin(), it);
    if (distance < 2) return -1;

    size_t last_match = static_cast<size_t>(distance - 2);

    int sum_goals = num_matches > last_match
                        ? team_data.home_goals[last_match]
                        : team_data.home_goals[last_match] -
                              team_data.home_goals[last_match - num_matches];
    int sum_against =
        num_matches > last_match
            ? team_data.home_against[last_match]
            : team_data.home_against[last_match] -
                  team_data.home_against[last_match - num_matches];

    size_t count = num_matches > last_match ? last_match + 1 : num_matches;
    out.goal_avg[LeagueStats::HOME] = (float)sum_goals / (float)count;
    out.goal_avg[LeagueStats::AWAY] = (float)sum_against / (float)count;
    out.goal_avg[LeagueStats::TOTAL] = 0.5f * (out.goal_avg[LeagueStats::HOME] +
                                               out.goal_avg[LeagueStats::AWAY]);
    out.match_count = (float)count;

    return (int)count;
}

int kwa::StatsProvider::getLeagueStatsBefore(int before_date,
                                             LeagueStats& out) const
{
    auto& team_data = m_overall_stats;
    assert(team_data.home_goals.size() > 0);
    auto it = std::lower_bound(team_data.home_dates.begin(),
                               team_data.home_dates.end(), before_date);

    auto distance = std::distance(team_data.home_dates.begin(), it);
    if (distance < 2) return -1;

    size_t last_match = static_cast<size_t>(distance - 2);
    int count = static_cast<int>(distance - 1);

    int sum_goals = team_data.home_goals[last_match];
    int sum_against = team_data.home_against[last_match];

    out.goal_avg[LeagueStats::HOME] = (float)sum_goals / (float)count;
    out.goal_avg[LeagueStats::AWAY] = (float)sum_against / (float)count;
    out.goal_avg[LeagueStats::TOTAL] = 0.5f * (out.goal_avg[LeagueStats::HOME] +
                                               out.goal_avg[LeagueStats::AWAY]);
    out.match_count = (float)count;

    return count;
}

int kwa::StatsProvider::getHomeStatsBefore(TeamId team, int before_date,
                                           size_t num_matches,
                                           TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    assert(team_data.home_goals.size() > 0);

    auto it = std::lower_bound(team_data.home_dates.begin(),
                               team_data.home_dates.end(), before_date);

    auto distance = std::distance(team_data.home_dates.begin(), it);
    if (distance < 1) return -1;

    size_t last_match = static_cast<size_t>(distance - 1);

    int sum_goals = num_matches > last_match
                        ? team_data.home_goals[last_match]
                        : team_data.home_goals[last_match] -
                              team_data.home_goals[last_match - num_matches];
    int sum_against =
        num_matches > last_match
            ? team_data.home_against[last_match]
            : team_data.home_against[last_match] -
                  team_data.home_against[last_match - num_matches];

    size_t count = num_matches > last_match ? last_match + 1 : num_matches;
    out.goal_avg[TeamStats::HOME] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::HOME] = (float)sum_against / (float)count;
    out.match_count[TeamStats::HOME] = (float)count;

    return (int)count;
}

int kwa::StatsProvider::getGuestStatsBefore(TeamId team, int before_date,
                                            size_t num_matches,
                                            TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    assert(team_data.away_goals.size() > 0);

    auto it = std::lower_bound(team_data.guest_dates.begin(),
                               team_data.guest_dates.end(), before_date);

    auto distance = std::distance(team_data.guest_dates.begin(), it);
    if (distance < 1) return -1;

    size_t last_match = static_cast<size_t>(distance - 1);

    int sum_goals = num_matches > last_match
                        ? team_data.away_goals[last_match]
                        : team_data.away_goals[last_match] -
                              team_data.away_goals[last_match - num_matches];
    int sum_against =
        num_matches > last_match
            ? team_data.away_against[last_match]
            : team_data.away_against[last_match] -
                  team_data.away_against[last_match - num_matches];

    size_t count = num_matches > last_match ? last_match + 1 : num_matches;
    out.goal_avg[TeamStats::AWAY] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::AWAY] = (float)sum_against / (float)count;
    out.match_count[TeamStats::AWAY] = (float)count;

    return (int)count;
}

int kwa::StatsProvider::getHomeStatsBefore(TeamId team, int before_date,
                                           TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    assert(team_data.home_goals.size() > 0);

    auto it = std::lower_bound(team_data.home_dates.begin(),
                               team_data.home_dates.end(), before_date);

    auto distance = std::distance(team_data.home_dates.begin(), it);
    if (distance < 1) return -1;

    size_t last_match = static_cast<size_t>(distance - 1);
    int count = static_cast<int>(distance);

    int sum_goals = team_data.home_goals[last_match];
    int sum_against = team_data.home_against[last_match];

    out.goal_avg[TeamStats::HOME] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::HOME] = (float)sum_against / (float)count;
    out.match_count[TeamStats::HOME] = (float)count;

    return count;
}

int kwa::StatsProvider::getGuestStatsBefore(TeamId team, int before_date,
                                            TeamStats& out) const
{
    auto& team_data = getTeamData(team);
    assert(team_data.away_goals.size() > 0);

    auto it = std::lower_bound(team_data.guest_dates.begin(),
                               team_data.guest_dates.end(), before_date);

    auto distance = std::distance(team_data.guest_dates.begin(), it);
    if (distance < 1) return -1;

    size_t last_match = static_cast<size_t>(distance - 1);
    int count = static_cast<int>(distance);

    int sum_goals = team_data.away_goals[last_match];
    int sum_against = team_data.away_against[last_match];

    out.goal_avg[TeamStats::AWAY] = (float)sum_goals / (float)count;
    out.against_avg[TeamStats::AWAY] = (float)sum_against / (float)count;
    out.match_count[TeamStats::AWAY] = (float)count;

    return count;
}

void kwa::StatsProvider::registerTeam(TeamId team_id)
{
    if ((size_t)team_id >= m_team_data.size())
        m_team_data.resize((size_t)team_id + 1);
}

auto kwa::StatsProvider::getTeamData(TeamId team_id) -> PerTeamData &
{
    assert(m_team_data.size() > (size_t)team_id);
    return m_team_data[(size_t)team_id];
}

auto kwa::StatsProvider::getTeamData(TeamId team_id) const -> const PerTeamData
    &
{
    assert(m_team_data.size() > (size_t)team_id);
    return m_team_data[(size_t)team_id];
}
