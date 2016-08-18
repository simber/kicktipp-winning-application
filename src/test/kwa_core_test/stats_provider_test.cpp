#include "kwa_core_test.h"

namespace
{
static const unsigned int MAX_TEAMS = 20;

kwa::MatchData createRandomMatch(int max_team_id, int max_goals)
{
    int year = 1990 + (std::rand() % 25);
    int month = (std::rand() % 13);
    int day = (std::rand() % 32);
    kwa::MatchData out;
    out.day = kwa::match_date(year, month, day);
    out.home_team = std::rand() % (max_team_id + 1);
    out.guest_team = std::rand() % (max_team_id + 1);
    out.home_goals = std::rand() % (max_goals + 1);
    out.guest_goals = std::rand() % (max_goals + 1);

    return out;
}

std::vector<kwa::MatchData> createRandomMatches(size_t count, int max_team_id,
                                                int max_goals)
{
    std::vector<kwa::MatchData> out;
    for (size_t i = 0; i < count; ++i) {
        out.emplace_back(createRandomMatch(max_team_id, max_goals));
    }

    std::sort(out.begin(), out.end(),
              [](auto lhs, auto rhs) { return lhs.day < rhs.day; });

    return out;
}

TEST(StatsProvider, constr_default_team_count_is_zero)
{
    kwa::StatsProvider provider{};
    EXPECT_EQ(0, provider.teamCount());
}

TEST(StatsProvider, constr_zero_team_count_is_zero)
{
    kwa::StatsProvider provider{0};
    EXPECT_EQ(0, provider.teamCount());
}

TEST(StatsProvider, constr_match_count_is_zero)
{
    {
        kwa::StatsProvider provider{};
        EXPECT_EQ(0, provider.matchCount());
    }

    {
        kwa::StatsProvider provider{0};
        EXPECT_EQ(0, provider.matchCount());
    }

    {
        unsigned int team_count =
            (static_cast<unsigned int>(std::rand()) % MAX_TEAMS) + 1;
        kwa::StatsProvider provider{team_count};
        EXPECT_EQ(0, provider.matchCount());
    }
}

TEST(StatsProvider, constr_random_greater_zero_team_count_is_set)
{
    unsigned int team_count =
        (static_cast<unsigned int>(std::rand()) % MAX_TEAMS) + 1;
    kwa::StatsProvider provider{team_count};
    EXPECT_EQ(team_count, provider.teamCount());
}

TEST(StatsProvider, clear_sets_team_count_to_zero)
{
    kwa::StatsProvider provider{2};
    provider.clear();
    EXPECT_EQ(0, provider.teamCount());
}

TEST(StatsProvider, clear_sets_match_count_to_zero)
{
    kwa::StatsProvider provider{2};
    provider.clear();
    EXPECT_EQ(0, provider.matchCount());
}

TEST(StatsProvider, set_team_count_greater_zero)
{
    unsigned int team_count =
        (static_cast<unsigned int>(std::rand()) % MAX_TEAMS) + 1;
    kwa::StatsProvider provider{};
    provider.setTeamCount(team_count);
    EXPECT_EQ(team_count, provider.teamCount());
}

TEST(StatsProvider, set_team_count_zero)
{
    kwa::StatsProvider provider{3};
    provider.setTeamCount(0);
    EXPECT_EQ(0, provider.teamCount());
}

TEST(StatsProvider, add_one_match_match_count_is_one)
{
    kwa::StatsProvider provider{};
    auto match = createRandomMatch(17, 9);
    provider.addMatches(match);
    EXPECT_EQ(1, provider.matchCount());
}

TEST(StatsProvider, add_multiple_matches_match_count_is_correct)
{
    kwa::StatsProvider provider{};
    size_t match_count = 2 + (std::rand() % 100);
    auto matches = createRandomMatches(match_count, 17, 9);
    provider.addMatches(matches);
    EXPECT_EQ(match_count, provider.matchCount());
}

TEST(StatsProvider, add_one_match_registeres_teams_automatically)
{
    {
        kwa::StatsProvider provider{};
        kwa::MatchData match;
        match.day = 0;
        match.home_team = 2;
        match.guest_team = 1;
        match.home_goals = 0;
        match.guest_goals = 0;
        provider.addMatches(match);
        EXPECT_EQ(3, provider.teamCount());
    }

    {
        kwa::StatsProvider provider{};
        kwa::MatchData match;
        match.day = 0;
        match.home_team = 2;
        match.guest_team = 5;
        match.home_goals = 0;
        match.guest_goals = 0;
        provider.addMatches(match);
        EXPECT_EQ(6, provider.teamCount());
    }

    {
        kwa::StatsProvider provider{};
        kwa::MatchData match;
        match.day = 0;
        match.home_team = 0;
        match.guest_team = 0;
        match.home_goals = 0;
        match.guest_goals = 0;
        provider.addMatches(match);
        EXPECT_EQ(1, provider.teamCount());
    }
}

TEST(StatsProvider, has_stats)
{
    kwa::StatsProvider provider{};

    EXPECT_FALSE(provider.hasHomeStats(2));
    EXPECT_FALSE(provider.hasGuestStats(4));

    kwa::MatchData match;
    match.day = 3;
    match.home_team = 2;
    match.guest_team = 4;
    match.home_goals = 0;
    match.guest_goals = 0;
    provider.addMatches(match);

    EXPECT_TRUE(provider.hasHomeStats(2));
    EXPECT_TRUE(provider.hasGuestStats(4));

    EXPECT_FALSE(provider.hasHomeStats(2, 3));
    EXPECT_FALSE(provider.hasGuestStats(4, 3));
}

TEST(StatsProvider, get_stats_one_match)
{
    kwa::StatsProvider provider{};
    kwa::MatchData match{0, 2, 1, 2, 1};
    provider.addMatches(match);

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getHomeStats(2, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(1.0f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getGuestStats(1, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(1.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(2.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }
}

TEST(StatsProvider, get_stats_multiple_matches)
{
    kwa::StatsProvider provider{};
    kwa::MatchData matches[] = {
        {0, 3, 2, 0, 1}, {1, 1, 2, 2, 3}, {2, 1, 2, 4, 1}, {3, 1, 3, 0, 0}};
    provider.addMatches(matches);

    {
        kwa::TeamStats stats;
        EXPECT_EQ(3, provider.getHomeStats(1, stats));
        EXPECT_FLOAT_EQ(3.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(4.0f / 3.0f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(3, provider.getGuestStats(2, stats));
        EXPECT_FLOAT_EQ(3.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(5.0f / 3.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(2.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }
}

TEST(StatsProvider, get_stats_max_matches)
{
    kwa::StatsProvider provider{};
    kwa::MatchData matches[] = {
        {0, 3, 2, 0, 1}, {1, 1, 2, 2, 3}, {2, 1, 2, 4, 1}, {3, 1, 3, 0, 0}};
    provider.addMatches(matches);

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getHomeStats(1, 1, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(0.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(0.0f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(3, provider.getHomeStats(1, 4, stats));
        EXPECT_FLOAT_EQ(3.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(4.0f / 3.0f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getGuestStats(2, 1, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(1.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(4.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(3, provider.getGuestStats(2, 5, stats));
        EXPECT_FLOAT_EQ(3.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(5.0f / 3.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(2.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }
}

TEST(StatsProvider, get_stats_before)
{
    kwa::StatsProvider provider{};
    kwa::MatchData matches[] = {
        {0, 3, 2, 0, 1}, {1, 1, 2, 2, 3}, {2, 1, 2, 4, 1}, {3, 1, 3, 0, 0}};
    provider.addMatches(matches);

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getHomeStatsBefore(1, 2, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(3.0f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(2, provider.getHomeStatsBefore(1, 3, stats));
        EXPECT_EQ(2.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_EQ(3.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_EQ(2.0f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getGuestStatsBefore(2, 1, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(1.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(0.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(2, provider.getGuestStatsBefore(2, 2, stats));
        EXPECT_FLOAT_EQ(2.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(1.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }
}

TEST(StatsProvider, get_stats_before_max_maches)
{
    kwa::StatsProvider provider{};
    kwa::MatchData matches[] = {
        {0, 3, 2, 0, 1}, {1, 1, 2, 2, 3}, {2, 1, 2, 4, 1}, {3, 1, 3, 0, 0}};
    provider.addMatches(matches);

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getHomeStatsBefore(1, 2, 1, stats));
        EXPECT_EQ(1.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_EQ(3.0f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(2, provider.getHomeStatsBefore(1, 4, 2, stats));
        EXPECT_FLOAT_EQ(2.0f, stats.match_count[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::HOME]);
        EXPECT_FLOAT_EQ(0.5f, stats.against_avg[kwa::TeamStats::HOME]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(1, provider.getGuestStatsBefore(2, 1, 2, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(1.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(0.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }

    {
        kwa::TeamStats stats;
        EXPECT_EQ(2, provider.getGuestStatsBefore(2, 4, 2, stats));
        EXPECT_FLOAT_EQ(2.0f, stats.match_count[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(2.0f, stats.goal_avg[kwa::TeamStats::AWAY]);
        EXPECT_FLOAT_EQ(3.0f, stats.against_avg[kwa::TeamStats::AWAY]);
    }
}

TEST(StatsProvider, get_league_stats)
{
    kwa::StatsProvider provider{};
    kwa::MatchData matches[] = {
        {0, 3, 2, 0, 1}, {1, 1, 2, 2, 3}, {2, 1, 2, 4, 1}, {3, 1, 3, 0, 0}};
    provider.addMatches(matches);

    kwa::LeagueStats stats;
    EXPECT_EQ(4, provider.getLeagueStats(stats));
    EXPECT_FLOAT_EQ(4.0f, stats.match_count);
    EXPECT_FLOAT_EQ(6.0f / 4.0f, stats.goal_avg[kwa::LeagueStats::HOME]);
    EXPECT_FLOAT_EQ(5.0f / 4.0f, stats.goal_avg[kwa::LeagueStats::AWAY]);
    EXPECT_FLOAT_EQ(11.0f / 8.0f, stats.goal_avg[kwa::LeagueStats::TOTAL]);
}

TEST(StatsProvider, get_league_stats_max_matches)
{
    kwa::StatsProvider provider{};
    kwa::MatchData matches[] = {
        {0, 3, 2, 0, 1}, {1, 1, 2, 2, 3}, {2, 1, 2, 4, 1}, {3, 1, 3, 0, 0}};
    provider.addMatches(matches);

    {
        kwa::LeagueStats stats;
        EXPECT_EQ(4, provider.getLeagueStats(4, stats));
        EXPECT_FLOAT_EQ(4.0f, stats.match_count);
        EXPECT_FLOAT_EQ(6.0f / 4.0f, stats.goal_avg[kwa::LeagueStats::HOME]);
        EXPECT_FLOAT_EQ(5.0f / 4.0f, stats.goal_avg[kwa::LeagueStats::AWAY]);
        EXPECT_FLOAT_EQ(11.0f / 8.0f, stats.goal_avg[kwa::LeagueStats::TOTAL]);
    }

    {
        kwa::LeagueStats stats;
        EXPECT_EQ(1, provider.getLeagueStats(1, stats));
        EXPECT_FLOAT_EQ(1.0f, stats.match_count);
        EXPECT_FLOAT_EQ(0.0f, stats.goal_avg[kwa::LeagueStats::HOME]);
        EXPECT_FLOAT_EQ(0.0f, stats.goal_avg[kwa::LeagueStats::AWAY]);
        EXPECT_FLOAT_EQ(0.0f, stats.goal_avg[kwa::LeagueStats::TOTAL]);
    }

    {
        kwa::LeagueStats stats;
        EXPECT_EQ(3, provider.getLeagueStats(3, stats));
        EXPECT_FLOAT_EQ(3.0f, stats.match_count);
        EXPECT_FLOAT_EQ(6.0f / 3.0f, stats.goal_avg[kwa::LeagueStats::HOME]);
        EXPECT_FLOAT_EQ(4.0f / 3.0f, stats.goal_avg[kwa::LeagueStats::AWAY]);
        EXPECT_FLOAT_EQ(10.0f / 6.0f, stats.goal_avg[kwa::LeagueStats::TOTAL]);
    }
}
}
