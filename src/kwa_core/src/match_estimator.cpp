#include "match_estimator.h"
#include "calculations.h"
#include <algorithm>

void kwa::MatchEstimator::addMatch(int date, const char* home_team,
                                   const char* guest_team, int home_goals,
                                   int guest_goals)
{
    kwa::MatchData m;
    m.day = date;
    m.home_team = m_team_register.registerTeam(home_team);
    m.guest_team = m_team_register.registerTeam(guest_team);
    m.home_goals = home_goals;
    m.guest_goals = guest_goals;

    m_matches.insert(std::upper_bound(m_matches.begin(), m_matches.end(), m,
                                      [](auto& lhs, auto& rhs) {
                                          return lhs.day < rhs.day;
                                      }),
                     m);
}

void kwa::MatchEstimator::recalculateTeamStatistics()
{
    m_team_statistics.clear();
    m_team_statistics.setTeamCount(m_team_register.size());
    m_team_statistics.addMatches(m_matches, 4);
}

void kwa::MatchEstimator::estimate(MatchEstimation& out, const char* home_team,
                                   const char* guest_team)
{
    estimate(out, m_team_register.getId(home_team),
             m_team_register.getId(guest_team));
}

void kwa::MatchEstimator::clear()
{
    m_team_statistics.clear();
    m_matches.clear();
    m_team_register.clear();
}

bool kwa::MatchEstimator::hasHomeStatistics(const char* team_name) const
{
    auto id = m_team_register.getId(team_name);
    if (id == INVALID_TEAM_ID) return false;
    return m_team_statistics.hasHomeStats(id, m_max_date);
}

bool kwa::MatchEstimator::hasGuestStatistics(const char* team_name) const
{
    auto id = m_team_register.getId(team_name);
    if (id == INVALID_TEAM_ID) return false;
    return m_team_statistics.hasGuestStats(id, m_max_date);
}

// namespace
// {
// void print_distr(gsl::span<const float> distr)
// {
//     char buffer[20];
//     for (int i = 0; i < distr.size(); ++i) {
//         std::sprintf(buffer, "%.2f", distr[i]);
//         std::cout << buffer << " ";
//     }
// }
// }

void kwa::MatchEstimator::estimate(MatchEstimation& out, TeamId home_id,
                                   TeamId guest_id)
{
    if (m_matches.size() != m_team_statistics.matchCount())
        recalculateTeamStatistics();

    kwa::LeagueStats league_stats;
    kwa::TeamStats home_stats, guest_stats;

    getStatistics(home_id, guest_id, home_stats, guest_stats, league_stats);

    kwa::GoalDistribution home_distr, guest_distr;
    float home_ev =
        kwa::CalculateHomeGoalEvSimple(home_stats, guest_stats, league_stats);
    float guest_ev =
        kwa::CalculateGuestGoalEvSimple(home_stats, guest_stats, league_stats);
    kwa::FillPoissonDistribution(home_distr, home_ev);
    kwa::FillPoissonDistribution(guest_distr, guest_ev);

    out.three_way_probabilities =
        kwa::CalculateThreeWayBet(home_distr, guest_distr, home_ev, guest_ev);

    auto best_bet = kwa::CalculateBestBet(home_distr, guest_distr, home_ev,
                                          guest_ev, m_system_points);

    out.best_result_bet_home_goals = best_bet.home_goals;
    out.best_result_bet_guest_goals = best_bet.guest_goals;
    out.best_result_bet_ev = best_bet.ev;
}

void kwa::MatchEstimator::getStatistics(TeamId home_id, TeamId guest_id,
                                        TeamStats& home_stats,
                                        TeamStats& guest_stats,
                                        LeagueStats& league_stats) const
{
    if (m_max_date < 0) {
        m_team_statistics.getHomeStats(home_id, home_stats);
        m_team_statistics.getGuestStats(guest_id, guest_stats);
        m_team_statistics.getLeagueStats(league_stats);
    } else {
        m_team_statistics.getHomeStatsBefore(home_id, m_max_date, home_stats);
        m_team_statistics.getGuestStatsBefore(guest_id, m_max_date,
                                              guest_stats);
        m_team_statistics.getLeagueStatsBefore(m_max_date, league_stats);
    }
}
