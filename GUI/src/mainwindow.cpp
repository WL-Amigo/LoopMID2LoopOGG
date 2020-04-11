#include "mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <QSettings>
#include "ui_mainwindow.h"

const quint32 SAMPLE_RATE = 44100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      configDialog(new ConfigDialog(nullptr)) {
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this,
            &MainWindow::onBrowseButtonPressed);
    connect(ui->checkBox, &QCheckBox::toggled, this,
            &MainWindow::onPreviewCheckBoxChanged);

    connect(ui->outputAmpSlider, &QSlider::valueChanged,
            this, &MainWindow::onOutputAmpValueChanged);
    connect(ui->outputAmpSpinbox, qOverload<int>(&QSpinBox::valueChanged),
            this, &MainWindow::onOutputAmpValueChanged);
    connect(this, &MainWindow::outputAmpChanged,
            ui->outputAmpSlider, &QSlider::setValue);
    connect(this, &MainWindow::outputAmpChanged,
            ui->outputAmpSpinbox, &QSpinBox::setValue);

    QSettings settings;
    if (!settings.value("outputDirectory").isNull()) {
        ui->lineEdit->setText(settings.value("outputDirectory").toString());
    }
    ui->checkBox->setChecked(settings.value("preview").toBool());
    emit outputAmpChanged(this->outputAmp());

    connect(ui->actionConfig, &QAction::triggered, this->configDialog,
            &ConfigDialog::show);
}

MainWindow::~MainWindow() { delete ui; }

int MainWindow::outputAmp() const {
    return QSettings().value("outputAmp", 100).toInt();
}

void MainWindow::setOutputAmp(int newOutputAmp) {
    QSettings().setValue("outputAmp", newOutputAmp);
    emit outputAmpChanged(newOutputAmp);
}

void MainWindow::onBrowseButtonPressed() {
    QString outputDirStr = QFileDialog::getExistingDirectory(
        this, tr("Select Output Direcotry..."));
    ui->lineEdit->setText(outputDirStr);
    QSettings settings;
    settings.setValue("outputDirectory", outputDirStr);
}

void MainWindow::onPreviewCheckBoxChanged() {
    QSettings s;
    s.setValue("preview", ui->checkBox->isChecked());
}

void MainWindow::onOutputAmpValueChanged(int newOutputAmp) {
    this->setOutputAmp(newOutputAmp);
}
