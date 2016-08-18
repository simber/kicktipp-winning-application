#pragma once

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Multiline_Output.H>
#include "../kwa_core/kwa_core.h"

template <typename FlGroupType>
class deactivate_auto_parent : public FlGroupType
{
public:
    template <typename... Args>
    deactivate_auto_parent(Args&&... args)
        : FlGroupType(std::forward<Args>(args)...)
    {
        static_assert(std::is_base_of<Fl_Group, FlGroupType>::value,
                      "Only use with subclasses of Fl_Group");
        Fl_Group::current(NULL);
    }
};

class MatchEstimationGui
{
public:
    explicit MatchEstimationGui();

    void show() { m_window.show(); }

private:
    kwa::MatchEstimator m_estimator;
    deactivate_auto_parent<Fl_Window> m_window;
    deactivate_auto_parent<Fl_Group> m_match_loader_group;
    Fl_Choice m_home_team_browser;
    Fl_Choice m_guest_team_browser;
    Fl_Button m_load_matches_btn;
    Fl_Button m_estimate_match_btn;
    Fl_Native_File_Chooser m_load_matches_chooser;
    Fl_Multiline_Output m_estimation_output;

    static void loadMatchesCb(Fl_Widget* w, void* user);
    static void estimateMatchCb(Fl_Widget* w, void* user);

    void updateTeamNames();
    void estimateMatch();
    bool loadMatches(const char* file);
    size_t teamCount() const { return m_estimator.teamRegister().size(); }
    std::vector<std::string> getTeamNames() const;
};
