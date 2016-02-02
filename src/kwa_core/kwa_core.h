#pragma once
#include "kwa_common.h"
#include "team_register.h"
#include "stats_provider.h"
#include "match_estimator.h"

namespace kwa
{
/**
 * @brief      Loads MatchData from a .csv file which MUST have the format used
 *             on www.football-data.co.uk and copies it to the MatchEstimator.
 *             The updateTeamStatistics() method of MatchEstimator will be
 *             called automatically.
 *
 * @param      estimator  The destination estimator.
 * @param[in]  file_name  file name.
 *
 * @return     true if success
 */
extern bool loadMatchesFromCSVFile(MatchEstimator& estimator,
                                   const char* file_name);
}
