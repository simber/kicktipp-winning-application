#include "kwa_core_test.h"

namespace
{
TEST(MatchEstimator, def_constr_is_empty)
{
    kwa::MatchEstimator estimator{};
    EXPECT_EQ(0, estimator.matchCount());
    EXPECT_EQ(0, estimator.teamRegister().size());
}

TEST(MatchEstimator, add_match_will_increase_match_count_by_one)
{
    kwa::MatchEstimator estimator{};
    estimator.addMatch(1, "Munich", "Bremen", 2, 1);
    estimator.recalculateTeamStatistics();

    EXPECT_EQ(1, estimator.matchCount());
    EXPECT_TRUE(estimator.hasHomeStatistics("Munich"));
    EXPECT_TRUE(estimator.hasGuestStatistics("Bremen"));
}

TEST(MatchEstimator, add_matches_will_increase_match_count_accordingly)
{
    kwa::MatchEstimator estimator{};
    estimator.addMatch(1, "Munich", "Bremen", 2, 1);
    estimator.addMatch(2, "Schalke", "Dortmund", 2, 1);
    estimator.addMatch(2, "Munich", "Dortmund", 0, 2);
    estimator.addMatch(5, "Bremen", "Hamburg", 5, 0);
    estimator.recalculateTeamStatistics();

    EXPECT_EQ(4, estimator.matchCount());
    EXPECT_TRUE(estimator.hasHomeStatistics("Munich"));
    EXPECT_TRUE(estimator.hasHomeStatistics("Schalke"));
    EXPECT_TRUE(estimator.hasHomeStatistics("Bremen"));
    EXPECT_TRUE(estimator.hasGuestStatistics("Bremen"));
    EXPECT_TRUE(estimator.hasGuestStatistics("Dortmund"));
    EXPECT_TRUE(estimator.hasGuestStatistics("Hamburg"));
}

TEST(MatchEstimator, estimate)
{
    kwa::MatchEstimator estimator{};
    estimator.addMatch(1, "Munich", "Bremen", 2, 1);
    estimator.addMatch(2, "Schalke", "Dortmund", 2, 1);
    estimator.addMatch(2, "Munich", "Dortmund", 0, 2);
    estimator.addMatch(5, "Bremen", "Hamburg", 5, 0);
    estimator.recalculateTeamStatistics();

    kwa::MatchEstimation estimation;
    estimator.estimate(estimation, "Munich", "Dortmund");
}

TEST(MatchEstimator, set_max_date)
{
    kwa::MatchEstimator estimator{};
    estimator.setMaxDate(12);
}

TEST(MatchEstimator, clear_sets_match_count_to_zero)
{
    kwa::MatchEstimator estimator{};
    estimator.addMatch(1, "Munich", "Bremen", 2, 1);
    estimator.addMatch(2, "Schalke", "Dortmund", 2, 1);
    estimator.addMatch(2, "Munich", "Dortmund", 0, 2);
    estimator.addMatch(5, "Bremen", "Hamburg", 5, 0);
    estimator.recalculateTeamStatistics();

    estimator.clear();
    EXPECT_EQ(0, estimator.matchCount());
    EXPECT_EQ(0, estimator.teamRegister().size());
}
}
