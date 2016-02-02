# kicktipp-winning-application
A football data analyser.
#### Intro:
A simple tool to predict outcomings of football matches via previous match results. The prediction provides odds for three way bets and additionally a best result bet for betting games like kicktipp.de where you get different points for tipping the right result, goal difference or match tendency.

The estimation is based on a simple computation of team strengths considering only previous scored and conceded goals. The team strength is used to build a poisson distribution for each team's goal scoring probabilities.
#### Usage:
Create an estimator instance:
```c++
kwa::MatchEstimator estimator;
```
Add matches to statistics database, either manual:
```c++
//add a single match
estimator.addMatch(kwa::match_date(2016, 1, 22), "Munich", "Hamburg", 2, 1);

//... add more matches

//build the team statistics
estimator.recalculateTeamStatistics();
```
or from a file:
```c++
//load matches from .csv file which must have the format used on www.football-data.co.uk 
kwa::loadMatchesFromCSVFile(estimator, "data/german_bundesliga_2015.csv");

//team statistics are built automatically
```
Now you can estimate a match based on the data loaded before:
```c++
kwa::MatchEstimation result;
estimator.estimate(result, "Munich", "Dortmund");
```
Note that both Teams MUST have statistics in the database to call the estimate function. This can be checked in advance via:
```c++
estimator.hasHomeStatistics("Munich");
estimator.hasGuestStatistics("Dortmund");
```
After calling estimate you can check the result kwa::MatchEstimation has the following members:
* three_way_probabilities[i]: The odds for the three different result types. i=0 home teams wins, i=1 draw, i=2 guest team wins.
* best_result_bet_home_goals, best_result_bet_guest_goals: Estimated best bet for a bet on kicktipp.de etc. 
* best_result_bet_ev: The estimated expected value of the best bet, in points.

#### Future:
* add more options to estimator (change points for betting game, how many matches will be used for statistics etc.
* add tool to simulate a whole season of betting to check estimation accuracy.
* time-variation of properties

### Used:
* [FLTK] (http://www.fltk.org)
* http://www.football-data.co.uk/
* Prediction and Retrospective Analysis of Soccer Matches in a League (Håvard Rue and Øyvind Salvesen, 1997)
* Modelling Association Football Scores and Inefficiencies in the Football Betting Market (Dixon and Coles, 1996)
