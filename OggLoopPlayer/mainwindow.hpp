#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QSlider>

#include "streamplayer.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    void configureFileDialog();

    // variables
private:
    StreamPlayer* player;
    QString cullentFileName;
    // widgets
    QProgressBar* positionBar;
    QSlider* seekPositionSelector;
    QSlider* volumeSlider;
    QLabel* cullentTimeLabel;
    QLabel* cullentFileNameLabel;
    QMenu* mainMenu;
    QMenuBar* mainMenuBar;
    QAction* openAct;
    QAction* quitAct;

    // action slots
private slots:
    void openFile();
    void play();
    void stop();
    void pause();
    void seek();
    void changeVolume(int value);
    void changeCullentPlayPosition();
    void changeTimeDisplayFormat();
    void emitQuit();

signals:
    void quit();
};

#endif  // MAINWINDOW_HPP
