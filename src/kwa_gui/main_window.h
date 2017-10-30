#include <QMainWindow>
#include <memory>
#include "kwa_core/match_estimator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> m_ui;
    kwa::MatchEstimator m_estimator;

    void clearMessages();
    void printMessage(const QString& msg);
    void onLoadMatches();
    void updateTeamChooser();
    void onChangeFile();
    void onEstimate();
};
