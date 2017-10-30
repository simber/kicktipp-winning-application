#pragma once

#include "gsl/gsl.h"
#include <array>
#include <stdlib.h>

namespace kwa {
// typedefs
using TeamId = int;
constexpr const TeamId INVALID_TEAM_ID = -1;
using ThreeWayBet = std::array<float, 3>;
constexpr const unsigned int MAX_GOALS = 10;
constexpr const unsigned int MAX_POSSIBLE_RESULTS = (MAX_GOALS + 1) * (MAX_GOALS + 1);
using GoalDistribution = std::array<float, MAX_GOALS + 1>;

// Data struct definitions
struct KicktippBet
{
    int home_goals;
    int guest_goals;
    float ev;
    float odds;
};

struct BetSystemPoints
{
    float result;
    float difference;
    float tendency;
};

struct TeamStats
{
    enum eLocation
    {
        HOME = 0,
        AWAY = 1,
        TOTAL = 2,
    };

    float goal_avg[3];
    float against_avg[3];
    float match_count[3];
};

struct MatchData
{
    int day;
    TeamId home_team;
    TeamId guest_team;
    int home_goals;
    int guest_goals;
};

struct LeagueStats
{
    enum eLocation
    {
        HOME = 0,
        AWAY = 1,
        TOTAL = 2,
    };

    float goal_avg[3];
    float match_count;
};

// helper inline functions
template<typename T, size_t N>
constexpr auto make_span(std::array<T, N>& arr, typename gsl::span<const T>::size_type count)
{
    assert((size_t)count <= arr.size());
    return gsl::span<T>(arr.data(), count);
}

template<typename T, size_t N>
constexpr auto make_span(std::array<T, N>&& arr, size_t count) = delete;

template<typename T, size_t N>
constexpr auto make_span(const std::array<T, N>& arr, typename gsl::span<const T>::size_type count)
{
    assert((size_t)count <= arr.size());
    return gsl::span<const T>(arr.data(), count);
}

inline int match_date(int year, int month, int day)
{
    int value = year * 10000;
    value += month * 100;
    value += day;
    return value;
}

inline int match_year(int val)
{
    return val / 10000;
}
inline int match_month(int val)
{
    return (val % 10000) / 100;
}
inline int match_day(int val)
{
    return (val % 100);
}
} // namespace kwa
