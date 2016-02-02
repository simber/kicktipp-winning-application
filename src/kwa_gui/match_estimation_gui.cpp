#include "match_estimation_gui.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

namespace
{
constexpr const int WINDOW_WIDTH = 800;
constexpr const int WINDOW_HEIGHT = 600;
constexpr const char* const WINDOW_TITLE = "Kicktipp Winning Application";

void positionRightTo(Fl_Widget& w, const Fl_Widget& other)
{
    int x = w.x();
    int y = w.y();

    w.position(x + other.x() + other.w(), y + other.y());
}

void positionBelowOf(Fl_Widget& w, const Fl_Widget& other)
{
    int x = w.x();
    int y = w.y();

    w.position(x + other.x(), y + other.y() + other.h());
}

void addRelativeTo(Fl_Widget& w, Fl_Group& parent)
{
    int x = w.x();
    int y = w.y();

    w.position(x + parent.x(), y + parent.y());
    parent.add(w);
}

double pround(double x, int precision)
{
    if (x == 0.) return x;
    int ex = (int)std::floor(log10(std::abs(x))) - precision + 1;
    double div = std::pow(10, ex);
    return std::floor(x / div + 0.5) * div;
}
}

MatchEstimationGui::MatchEstimationGui()
    : m_window{WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE},
      m_match_loader_group{10, 20, 200, 200},
      m_home_team_browser{0, 0, 150, 25}, m_guest_team_browser{0, 0, 150, 25},
      m_load_matches_btn{0, 0, 150, 25, "Load Matches"},
      m_estimate_match_btn{0, 10, 150, 25, "Estimate Match"},
      m_load_matches_chooser{}, m_estimation_output{10, 250, 400, 200}
{
    addRelativeTo(m_match_loader_group, m_window);
    addRelativeTo(m_load_matches_btn, m_match_loader_group);
    positionBelowOf(m_estimate_match_btn, m_load_matches_btn);
    m_match_loader_group.add(m_estimate_match_btn);
    m_load_matches_btn.callback(loadMatchesCb, this);
    m_estimate_match_btn.callback(estimateMatchCb, this);

    positionRightTo(m_home_team_browser, m_match_loader_group);
    m_window.add(m_home_team_browser);
    positionRightTo(m_guest_team_browser, m_home_team_browser);
    m_window.add(m_guest_team_browser);

    addRelativeTo(m_estimation_output, m_window);
}

void MatchEstimationGui::loadMatchesCb(Fl_Widget*, void* user)
{
    MatchEstimationGui* gui = reinterpret_cast<MatchEstimationGui*>(user);

    gui->m_load_matches_chooser.title("select one match data file");

    gui->m_load_matches_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    gui->m_load_matches_chooser.filter("CSV\t*.csv");
    gui->m_load_matches_chooser.directory("data/"); // default directory to use
    // Show native chooser
    switch (gui->m_load_matches_chooser.show()) {
    case -1:
        std::cout << "EROOR\t" << gui->m_load_matches_chooser.errmsg()
                  << std::endl;
        break; // ERROR
    case 1:
        break; // CANCEL
    default:
        gui->m_estimator.clear();
        std::cout << "PICKED\t" << gui->m_load_matches_chooser.filename()
                  << std::endl;
        ;
        if (gui->loadMatches(gui->m_load_matches_chooser.filename())) {
            std::cout << "LOADED\t" << gui->m_estimator.matchCount()
                      << " matches" << std::endl;
        } else {
            std::cout << "EROOR\t"
                      << " unable to load matches." << std::endl;
        }

        gui->updateTeamNames();
        break; // FILE CHOSEN
    }
}

void MatchEstimationGui::estimateMatch()
{
    kwa::TeamId home = m_home_team_browser.value();
    kwa::TeamId guest = m_guest_team_browser.value();

    if (home < 0 || guest < 0) return;

    kwa::MatchEstimation estimation;

    m_estimator.estimate(estimation, home, guest);

    std::stringstream strstr;

    strstr << m_estimator.teamRegister().teamName(home) << " : "
           << m_estimator.teamRegister().teamName(guest)
           << "\nbest: " << estimation.best_result_bet_home_goals << ":"
           << estimation.best_result_bet_guest_goals
           << " ev=" << pround(estimation.best_result_bet_ev, 3) << "\n";

    strstr << "1=" << pround(estimation.three_way_probabilities[0], 2)
           << " X=" << pround(estimation.three_way_probabilities[1], 2)
           << " 2=" << pround(estimation.three_way_probabilities[2], 2)
           << std::endl;

    auto str = strstr.str();
    m_estimation_output.value(str.c_str());
}

void MatchEstimationGui::estimateMatchCb(Fl_Widget*, void* user)
{
    MatchEstimationGui* gui = reinterpret_cast<MatchEstimationGui*>(user);

    gui->estimateMatch();
}

void MatchEstimationGui::updateTeamNames()
{
    m_home_team_browser.clear();
    m_guest_team_browser.clear();
    auto names = getTeamNames();

    if (names.size() < 2) return;

    for (auto& name : names) {
        m_home_team_browser.add(name.c_str());
        m_guest_team_browser.add(name.c_str());
    }

    m_home_team_browser.value(0);
    m_guest_team_browser.value(1);
}

bool MatchEstimationGui::loadMatches(const char* file)
{

    kwa::loadMatchesFromCSVFile(m_estimator, file);

    return true;
}

std::vector<std::string> MatchEstimationGui::getTeamNames() const
{
    std::vector<std::string> out;
    for (int i = 0; i < (int)m_estimator.teamRegister().size(); ++i) {
        out.push_back(m_estimator.teamRegister().teamName(i));
    }
    return out;
}
