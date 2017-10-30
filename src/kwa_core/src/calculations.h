#pragma once
#include "kwa_common.h"
#include <vector>

namespace kwa {
extern float CalculateBetEV(int home_goals, int guest_goals, gsl::span<const float> home_distr,
                            gsl::span<const float> guest_distr, float home_ev, float guest_ev,
                            const BetSystemPoints& system_points);

extern auto CalculateBestBet(const GoalDistribution& home_distr,
                             const GoalDistribution& guest_distr, float home_ev, float guest_ev,
                             const BetSystemPoints& system_points) -> KicktippBet;

extern auto CalculateBestBet(const GoalDistribution& home_distr,
                             const GoalDistribution& guest_distr, float home_ev, float guest_ev,
                             const BetSystemPoints& system_points, int home_cap, int guest_cap)
    -> KicktippBet;

extern auto CalculateThreeWayBet(const GoalDistribution& home_distr,
                                 const GoalDistribution& guest_distr) -> ThreeWayBet;

extern auto CalculateThreeWayBet(const GoalDistribution& home_distr,
                                 const GoalDistribution& guest_distr, float home_ev, float guest_ev)
    -> ThreeWayBet;

extern int FillPoissonDistribution(gsl::span<float> out, float lmbda, float thresh_hold = 0.0f);

extern float CalculateGoalEv(float attacker_goal_avg, float defender_against_avg,
                             float league_goal_avg);

extern float CalculateHomeGoalEvSimple(const TeamStats& home_team, const TeamStats& guest_team,
                                       const LeagueStats& league);

extern float CalculateGuestGoalEvSimple(const TeamStats& home_team, const TeamStats& guest_team,
                                        const LeagueStats& league);

extern float CalculateHomeGoalEvCombined(const TeamStats& home_team, const TeamStats& guest_team,
                                         const LeagueStats& league, float w1, float w2);

extern float CalculateGuestGoalEvCombined(const TeamStats& home_team, const TeamStats& guest_team,
                                          const LeagueStats& league, float w1, float w2);

extern auto CalculateTeamStats(gsl::span<const MatchData> matches, int team_count,
                               LeagueStats& league_stats_out) -> std::vector<TeamStats>;
} // namespace kwa
