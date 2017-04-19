#include "mainwindow.hpp"

#include <QAction>
#include <QHBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include <QMessageBox>
#include <QSettings>
#include <QStringBuilder>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // init player
    player = new StreamPlayer(this);
    cullentFileName = "";
    connect(player, SIGNAL(notify()), this, SLOT(changeCullentPlayPosition()));

    // init positionBar
    positionBar = new QProgressBar(this);
    positionBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    positionBar->setFixedHeight(8);
    positionBar->setMinimum(0);
    positionBar->setTextVisible(false);

    // init seekPositionSelector
    seekPositionSelector = new QSlider(Qt::Horizontal, this);
    seekPositionSelector->setMaximum(100);
    seekPositionSelector->setMinimum(0);
    seekPositionSelector->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Preferred);

    // init volumeSlider
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setMaximum(100);
    volumeSlider->setMinimum(0);
    volumeSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    volumeSlider->setFixedWidth(100);
    volumeSlider->setValue(100);
    connect(volumeSlider, SIGNAL(valueChanged(int)), this,
            SLOT(changeVolume(int)));

    // set menu contents
    mainMenuBar = new QMenuBar();

    mainMenu = new QMenu(tr("&File"), mainMenuBar);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a ogg vorbis file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    quitAct = new QAction(tr("&Exit"), this);
    quitAct->setShortcut(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Exit from this application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(emitQuit()));

    mainMenu->addAction(openAct);
    mainMenu->addSeparator();
    mainMenu->addAction(quitAct);

    mainMenuBar->addMenu(mainMenu);

    this->setMenuBar(mainMenuBar);

    // create buttons
    QPushButton* playButton = new QPushButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playButton, SIGNAL(clicked()), this, SLOT(play()));

    QPushButton* stopButton = new QPushButton(this);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    QPushButton* pauseButton = new QPushButton(this);
    pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(pause()));

    QPushButton* seekButton = new QPushButton(this);
    seekButton->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    connect(seekButton, SIGNAL(clicked()), this, SLOT(seek()));

    // init label
    cullentTimeLabel = new QLabel("--:--/--:--", this);
    cullentTimeLabel->setSizePolicy(QSizePolicy::Minimum,
                                    QSizePolicy::Preferred);
    cullentFileNameLabel = new QLabel("---", this);
    cullentFileNameLabel->setSizePolicy(QSizePolicy::Maximum,
                                        QSizePolicy::Preferred);

    // layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(positionBar);
    mainLayout->addWidget(seekPositionSelector);

    QHBoxLayout* labelsHLayout = new QHBoxLayout();
    labelsHLayout->addWidget(cullentFileNameLabel, 0, Qt::AlignLeft);
    labelsHLayout->addWidget(cullentTimeLabel, 0, Qt::AlignRight);
    mainLayout->addLayout(labelsHLayout);

    QHBoxLayout* volumeHLayout = new QHBoxLayout();
    volumeHLayout->addWidget(new QLabel(tr("volume"), this), 0, Qt::AlignLeft);
    volumeHLayout->addWidget(volumeSlider, 0, Qt::AlignRight);
    mainLayout->addLayout(volumeHLayout);

    QHBoxLayout* buttonsHLayout = new QHBoxLayout();
    buttonsHLayout->addWidget(playButton);
    buttonsHLayout->addWidget(pauseButton);
    buttonsHLayout->addWidget(stopButton);
    buttonsHLayout->addWidget(seekButton);
    mainLayout->addLayout(buttonsHLayout);

    QWidget* centralWindow = new QWidget();
    centralWindow->setLayout(mainLayout);

    this->setCentralWidget(centralWindow);
}

MainWindow::~MainWindow() {}

void MainWindow::configureFileDialog() {}

// action slots

void MainWindow::openFile() {
    QString fileName;
    QSettings settings;

    if (!(fileName = QFileDialog::getOpenFileName(
              this, "select ogg vorbis file with loop tag...",
              settings.value("LastDirectory", "").toString(),
              tr("ogg vorbis audio (*.ogg)")))
             .isEmpty()) {
        cullentFileName = fileName;

        // record last selected directory contains file of cullentFileName
        QFileInfo fi(cullentFileName);
        settings.setValue("LastDirectory", fi.dir().absolutePath());

        if (!player->open(cullentFileName)) {
            QMessageBox::warning(this, "Cannot open file",
                                 QString("Couldn't open file: ")
                                     + cullentFileName + "\nReason : "
                                     + player->getErrorReason(),
                                 QMessageBox::Ok, QMessageBox::Ok);
            return;
        }

        QString title = player->getTitle();
        if (title.isEmpty()) {
            QFileInfo fileInfo = QFileInfo(cullentFileName);
            title = fileInfo.fileName();
        }

        // set title label
        cullentFileNameLabel->setText(title);

        // set time label
        QString cullentTimeText;
        int totalSecond
            = player->getAllSampleNum() / player->getAudioFormat().sampleRate();
        cullentTimeText.sprintf("0:00/%d:%02d", totalSecond / 60,
                                totalSecond % 60);
        cullentTimeLabel->setText(cullentTimeText);

        // set seek position selector maximum value
        seekPositionSelector->setMaximum(player->getAllSampleNum());

        // set position bar maximum value
        positionBar->setMaximum(player->getAllSampleNum());
        // FIXME : if allSampleNum > maximum of int value, this cause overflow
    }
}

void MainWindow::play() { player->play(); }

void MainWindow::stop() { player->stop(); }

void MainWindow::pause() { player->pause(); }

void MainWindow::seek() { player->seekTo(seekPositionSelector->value()); }

void MainWindow::changeVolume(int value) { player->changeVolume(value); }

void MainWindow::changeCullentPlayPosition() {
    qint64 cullentPosition = player->getCullentPositionAbout();
    qint64 allSamples = player->getAllSampleNum();
    int totalSecond = allSamples / player->getAudioFormat().sampleRate();
    int cullentSecond = cullentPosition / player->getAudioFormat().sampleRate();

    // change progress bar value
    positionBar->setValue(cullentPosition);
    positionBar->repaint();

    // change time display label
    QString cullentTimeText = "";
    cullentTimeText.sprintf("%d:%02d/%d:%02d", cullentSecond / 60,
                            cullentSecond % 60, totalSecond / 60,
                            totalSecond % 60);
    cullentTimeLabel->setText(cullentTimeText);
}

void MainWindow::changeTimeDisplayFormat() {}

void MainWindow::emitQuit() { emit quit(); }
