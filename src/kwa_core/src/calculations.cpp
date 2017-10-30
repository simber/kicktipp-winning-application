#include "calculations.h"

static int factorial(int n)
{
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

static float poisson_prop(float lambda, int k)
{
    return powf(lambda, (float)k) / (float)factorial(k) * expf(-lambda);
}

static float poisson_correction_factor(int home_goals, int guest_goals,
                                       float home_ev, float guest_ev)
{
    if (home_goals > 1 || guest_goals > 1)
        return 1.0f;
    else if (home_goals == 0 && guest_goals == 0)
        return 1.0f + 0.1f * home_ev * guest_ev;
    else if (home_goals == 1 && guest_goals == 1)
        return 1.1f;
    else if (home_goals == 1 && guest_goals == 0)
        return 1.0f - 0.1f * guest_ev;
    else if (home_goals == 0 && guest_goals == 1)
        return 1.0f - 0.1f * home_ev;

    return 1.0f;
}

float kwa::CalculateBetEV(int home_goals, int guest_goals,
                          gsl::span<const float> home_distr,
                          gsl::span<const float> guest_distr, float home_ev,
                          float guest_ev, const BetSystemPoints& system_points)
{
    assert(home_goals < home_distr.size());
    assert(guest_goals < guest_distr.size());

    float ev = 0.0f;
    int diff = home_goals - guest_goals;
    for (int ia = 0; ia < home_distr.size(); ++ia) {
        for (int ib = 0; ib < guest_distr.size(); ++ib) {
            int act_diff = ia - ib;

            float points = 0.0f;

            // correct tip
            if (ia == home_goals && ib == guest_goals)
                points = system_points.result;

            // correct difference
            else if (act_diff == diff)
                points = diff == 0 ? system_points.tendency
                                   : system_points.difference;

            // correct tendency
            else if (act_diff * diff > 0)
                points = diff == 0 ? 0 : system_points.tendency;

            float prop = home_distr[ia] * guest_distr[ib];
            prop *= poisson_correction_factor(ia, ib, home_ev, guest_ev);
            ev += prop * points;
        }
    }
    return ev;
}

kwa::KicktippBet kwa::CalculateBestBet(const GoalDistribution& home_distr,
                                       const GoalDistribution& guest_distr,
                                       float home_ev, float guest_ev,
                                       const BetSystemPoints& system_points,
                                       int home_cap, int guest_cap)
{
    assert(home_cap >= 0 && home_cap <= (int)home_distr.size());
    assert(guest_cap >= 0 && guest_cap <= (int)guest_distr.size());

    KicktippBet out;
    out.home_goals = 0;
    out.guest_goals = 0;
    out.ev = 0.0f;
    out.odds = 0.0f;

    float ev = 0.0f;
    for (int i = 0; i < home_cap; ++i) {
        for (int j = 0; j < guest_cap; ++j) {
            ev = CalculateBetEV(i, j, make_span(home_distr, home_cap),
                                {guest_distr.data(), guest_cap}, home_ev,
                                guest_ev, system_points);
            if (ev > out.ev) {
                out.home_goals = i;
                out.guest_goals = j;
                out.ev = ev;
            }
        }
    }

    out.odds = home_distr[(size_t)out.home_goals] *
               guest_distr[(size_t)out.guest_goals];
    out.odds *= poisson_correction_factor(out.home_goals, out.guest_goals,
                                          home_ev, guest_ev);
    return out;
}

auto kwa::CalculateThreeWayBet(const GoalDistribution& home_distr,
                               const GoalDistribution& guest_distr)
    -> ThreeWayBet
{
    ThreeWayBet out{};

    for (size_t i = 0; i < home_distr.size(); ++i) {
        for (size_t j = 0; j < guest_distr.size(); ++j) {

            float odds = home_distr[i] * guest_distr[j];
            if (i > j)
                out[0] += odds;
            else if (i == j)
                out[1] += odds;
            else
                out[2] += odds;
        }
    }
    return out;
}

auto kwa::CalculateThreeWayBet(const GoalDistribution& home_distr,
                               const GoalDistribution& guest_distr,
                               float home_ev, float guest_ev) -> ThreeWayBet
{
    ThreeWayBet out{};

    for (size_t i = 0; i < home_distr.size(); ++i) {
        for (size_t j = 0; j < guest_distr.size(); ++j) {

            float odds = home_distr[i] * guest_distr[j];
            odds *=
                poisson_correction_factor((int)i, (int)j, home_ev, guest_ev);
            if (i > j)
                out[0] += odds;
            else if (i == j)
                out[1] += odds;
            else
                out[2] += odds;
        }
    }
    return out;
}

int kwa::FillPoissonDistribution(gsl::span<float> out, float lmbda,
                                 float thresh_hold /*= 0.0f*/)
{
    int cap = 1;
    for (int i = 0; i < (int)out.size(); ++i) {
        out[i] = poisson_prop(lmbda, i);
        if (out[i] > thresh_hold) ++cap;
    }
    return cap;
}

extern float kwa::CalculateGoalEv(float attacker_goal_avg,
                                  float defender_against_avg,
                                  float league_goal_avg)
{
    return attacker_goal_avg * defender_against_avg / league_goal_avg;
}

float kwa::CalculateHomeGoalEvSimple(const TeamStats& home_team,
                                     const TeamStats& guest_team,
                                     const LeagueStats& league)
{
    return CalculateGoalEv(home_team.goal_avg[TeamStats::HOME],
                           guest_team.against_avg[TeamStats::AWAY],
                           league.goal_avg[LeagueStats::HOME]);
}

float kwa::CalculateGuestGoalEvSimple(const TeamStats& home_team,
                                      const TeamStats& guest_team,
                                      const LeagueStats& league)
{
    return CalculateGoalEv(guest_team.goal_avg[TeamStats::AWAY],
                           home_team.against_avg[TeamStats::HOME],
                           league.goal_avg[LeagueStats::AWAY]);
}

float kwa::CalculateHomeGoalEvCombined(const TeamStats& home_team,
                                       const TeamStats& guest_team,
                                       const LeagueStats& league, float w1,
                                       float w2)
{
    float a = CalculateGoalEv(home_team.goal_avg[TeamStats::HOME],
                              guest_team.against_avg[TeamStats::AWAY],
                              league.goal_avg[LeagueStats::HOME]);

    float b = CalculateGoalEv(home_team.goal_avg[TeamStats::TOTAL],
                              guest_team.against_avg[TeamStats::TOTAL],
                              league.goal_avg[LeagueStats::TOTAL]);

    return w1 * a + w2 * b;
}

float kwa::CalculateGuestGoalEvCombined(const TeamStats& home_team,
                                        const TeamStats& guest_team,
                                        const LeagueStats& league, float w1,
                                        float w2)
{
    float a = CalculateGoalEv(guest_team.goal_avg[TeamStats::AWAY],
                              home_team.against_avg[TeamStats::HOME],
                              league.goal_avg[LeagueStats::AWAY]);

    float b = CalculateGoalEv(guest_team.goal_avg[TeamStats::TOTAL],
                              home_team.against_avg[TeamStats::TOTAL],
                              league.goal_avg[LeagueStats::TOTAL]);

    return w1 * a + w2 * b;
}

std::vector<kwa::TeamStats>
kwa::CalculateTeamStats(gsl::span<const MatchData> matches, int team_count,
                        LeagueStats& league_stats_out)
{
    std::vector<TeamStats> out((size_t)team_count);
    std::memset(&league_stats_out, 0, sizeof(league_stats_out));

    for (auto& match : matches) {
        assert(match.home_team < team_count);
        assert(match.guest_team < team_count);
        TeamStats& home = out[(size_t)match.home_team];
        TeamStats& guest = out[(size_t)match.guest_team];

        home.match_count[TeamStats::HOME] += 1;
        home.goal_avg[TeamStats::HOME] += match.home_goals;
        home.against_avg[TeamStats::HOME] += match.guest_goals;

        guest.match_count[TeamStats::AWAY] += 1;
        guest.goal_avg[TeamStats::AWAY] += match.guest_goals;
        guest.against_avg[TeamStats::AWAY] += match.home_goals;

        league_stats_out.goal_avg[LeagueStats::HOME] += match.home_goals;
        league_stats_out.goal_avg[LeagueStats::AWAY] += match.guest_goals;
    }

    league_stats_out.match_count = (float)matches.size();
    league_stats_out.goal_avg[LeagueStats::HOME] /=
        league_stats_out.match_count;
    league_stats_out.goal_avg[LeagueStats::AWAY] /=
        league_stats_out.match_count;
    league_stats_out.goal_avg[LeagueStats::TOTAL] =
        0.5f * (league_stats_out.goal_avg[LeagueStats::HOME] +
                league_stats_out.goal_avg[LeagueStats::AWAY]);

    for (auto& team : out) {
        team.match_count[TeamStats::TOTAL] = team.match_count[TeamStats::HOME] +
                                             team.match_count[TeamStats::AWAY];
        team.goal_avg[TeamStats::TOTAL] =
            team.goal_avg[TeamStats::HOME] + team.goal_avg[TeamStats::AWAY];
        team.against_avg[TeamStats::TOTAL] = team.against_avg[TeamStats::HOME] +
                                             team.against_avg[TeamStats::AWAY];
        team.goal_avg[TeamStats::HOME] /= team.match_count[TeamStats::HOME];
        team.goal_avg[TeamStats::AWAY] /= team.match_count[TeamStats::AWAY];
        team.goal_avg[TeamStats::TOTAL] /= team.match_count[TeamStats::TOTAL];
        team.against_avg[TeamStats::HOME] /= team.match_count[TeamStats::HOME];
        team.against_avg[TeamStats::AWAY] /= team.match_count[TeamStats::AWAY];
        team.against_avg[TeamStats::TOTAL] /=
            team.match_count[TeamStats::TOTAL];
    }

    return out;
}

kwa::KicktippBet kwa::CalculateBestBet(const GoalDistribution& home_distr,
                                       const GoalDistribution& guest_distr,
                                       float home_ev, float guest_ev,
                                       const BetSystemPoints& system_points)
{
    return CalculateBestBet(home_distr, guest_distr, home_ev, guest_ev,
                            system_points, (int)home_distr.size(),
                            (int)guest_distr.size());
}
