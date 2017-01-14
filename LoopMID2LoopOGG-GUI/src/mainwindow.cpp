#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>

const quint32 SAMPLE_RATE = 44100;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onBrowseButtonPressed);
    connect(ui->checkBox, &QCheckBox::toggled, this, &MainWindow::onPreviewCheckBoxChanged);

    QSettings settings;
    if(!settings.value("outputDirectory").isNull()){
        ui->lineEdit->setText(settings.value("outputDirectory").toString());
    }
    ui->checkBox->setChecked(settings.value("preview").toBool());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBrowseButtonPressed(){
    QString outputDirStr = QFileDialog::getExistingDirectory(this, tr("Select Output Direcotry..."));
    ui->lineEdit->setText(outputDirStr);
    QSettings settings;
    settings.setValue("outputDirectory", outputDirStr);
}

void MainWindow::onPreviewCheckBoxChanged(){
    QSettings s;
    s.setValue("preview", ui->checkBox->isChecked());
}

