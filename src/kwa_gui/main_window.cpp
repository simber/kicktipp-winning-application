#include "main_window.h"
#include "ui_main_window.h"
#include <QScrollBar>
#include "kwa_core/kwa_core.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent /*= nullptr*/)
    : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    printMessage("Match database is empty.");
    connect(m_ui->loadMatchesButton, &QPushButton::clicked, this, &MainWindow::onLoadMatches);
    connect(m_ui->changeDirButton, &QPushButton::clicked, this, &MainWindow::onChangeFile);
    connect(m_ui->estimateButton, &QPushButton::clicked, this, &MainWindow::onEstimate);
}

MainWindow::~MainWindow()
{
}

void MainWindow::clearMessages()
{
    m_ui->plainTextEdit->clear();
}

void MainWindow::printMessage(const QString& msg)
{
    if(msg.isEmpty()) return;
    const int end = (msg.at(msg.length() - 1) == '\n') ? msg.length() - 1 : msg.length();
    m_ui->plainTextEdit->appendPlainText(msg.left(end));
    m_ui->plainTextEdit->verticalScrollBar()->setValue(
        m_ui->plainTextEdit->verticalScrollBar()->maximum());
}

void MainWindow::onLoadMatches()
{
    clearMessages();
    const auto current_match_count = m_estimator.matchCount();
    if(!kwa::loadMatchesFromCSVFile(m_estimator, m_ui->lineEdit->text().toStdString().c_str())) {
        printMessage("[ERROR] Could not load matches from file.");
        return;
    }
    const auto loaded_matches = m_estimator.matchCount() - current_match_count;
    printMessage("[INFO] New matches loaded: " + QString::number(loaded_matches));
    printMessage("[INFO] Total matches in database: " + QString::number(m_estimator.matchCount()));
    updateTeamChooser();
}

void MainWindow::updateTeamChooser()
{
    m_ui->homeTeamBox->clear();
    m_ui->guestTeamBox->clear();

    const auto& team_register = m_estimator.teamRegister();
    for(size_t i = 0; i < team_register.size(); ++i) {
        const kwa::TeamId team_id = static_cast<int>(i);
        if(m_estimator.hasHomeStatistics(team_register.teamName(team_id).c_str())) {
            m_ui->homeTeamBox->addItem(QString::fromStdString(team_register.teamName(team_id)));
        }
        if(m_estimator.hasGuestStatistics(team_register.teamName(team_id).c_str())) {
            m_ui->guestTeamBox->addItem(QString::fromStdString(team_register.teamName(team_id)));
        }
    }
    if(m_ui->homeTeamBox->count() > 0 && m_ui->guestTeamBox->count() > 0) {
        m_ui->homeTeamBox->setEnabled(true);
        m_ui->guestTeamBox->setEnabled(true);
        m_ui->estimateButton->setEnabled(true);
    }
}

void MainWindow::onChangeFile()
{
    const auto new_file =
        QFileDialog::getOpenFileName(this, "Select match file", m_ui->lineEdit->text());
    if(new_file == "") return;
    m_ui->lineEdit->setText(new_file);
    m_ui->loadMatchesButton->setEnabled(true);
}

void MainWindow::onEstimate()
{
    kwa::MatchEstimation estimation;

    const auto home_team = m_ui->homeTeamBox->currentText();
    const auto guest_team = m_ui->guestTeamBox->currentText();

    m_estimator.estimate(estimation, home_team.toStdString().c_str(),
                         guest_team.toStdString().c_str());

    clearMessages();
    printMessage("Estimation for match: " + home_team + " : " + guest_team);
    printMessage("three way: " + QString::number(estimation.three_way_probabilities[0]) + " " +
                 QString::number(estimation.three_way_probabilities[1]) + " " +
                 QString::number(estimation.three_way_probabilities[2]));
    printMessage("kicktipp: " + QString::number(estimation.best_result_bet_home_goals) + ":" +
                 QString::number(estimation.best_result_bet_guest_goals) +
                 " ev=" + QString::number(estimation.best_result_bet_ev));
}
