#include "includes/ConfigDialog.hpp"
#include "ui_ConfigDialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSettings>

ConfigDialog::ConfigDialog(QWidget* parent)
    : QWidget(parent), ui(new Ui::ConfigDialog) {
    ui->setupUi(this);

    // setup config selector buttons
    setupConfigSelectorButtons();
    ui->pagesWidget->setCurrentIndex(0);

    // connect behavior
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &ConfigDialog::saveSettingsAndClose);

    // restore current settings
    restoreSettingsToUI();
}

ConfigDialog::~ConfigDialog() { delete ui; }

void ConfigDialog::setupConfigSelectorButtons() {
    // connect each config selector button to page
    this->connectCSBToPage(ui->CSBOutput, ui->CPOutput);
    this->connectCSBToPage(ui->CSBTiMidity, ui->CPTiMidity);
    this->connectCSBToPage(ui->CSBEffect, ui->CPEffect);
    this->connectCSBToPage(ui->CSBEncoder, ui->CPEncoder);
    this->connectCSBToPage(ui->CSBAbout, ui->CPAbout);
}

void ConfigDialog::connectCSBToPage(QToolButton *toolButton, QWidget* targetPageWidget){
    // connect config selector button to page specified
    CDChangePageSlotObject* cpso = new CDChangePageSlotObject(this, ui->pagesWidget, ui->pagesWidget->indexOf(targetPageWidget));
    connect(toolButton, &QToolButton::clicked, cpso, &CDChangePageSlotObject::changePage);
    connect(this, &ConfigDialog::destroyed, cpso, &CDChangePageSlotObject::deleteLater);
}

void ConfigDialog::restoreSettingsToUI() {
    // restore all settings to correspond UI elements
    QSettings s;
    // --- output section
    QString temp = s.value("output/fileType").toString();
    if (temp == "ogg") {
        ui->OFFOggRadio->setChecked(true);
    } else if (temp == "wav") {
        ui->OFFWaveRadio->setChecked(true);
    }
    temp = s.value("output/mode").toString();
    if (temp == "optimized") {
        ui->OMOptimizedRadio->setChecked(true);
    } else if (temp == "soundtrack") {
        ui->OMSoundtrackRadio->setChecked(true);
    }
    ui->OMOOMinSamples->setValue(
        s.value("output/maxSamplesAfterLoopEnd").toInt());
    ui->OMOSFOStartTime->setValue(s.value("output/fadeoutStartSec").toDouble());
    ui->OMOSFOLength->setValue(s.value("output/fadeoutLengthSec").toDouble());
    ui->OMOSNumLoop->setValue(s.value("output/loopNumber").toInt());
}

void ConfigDialog::saveSettingsAndClose() {
    // save all settings
    QSettings s;
    // --- output section
    if (ui->OFFOggRadio->isChecked()) {
        s.setValue("output/fileType", "ogg");
    } else if (ui->OFFWaveRadio->isChecked()) {
        s.setValue("output/fileType", "wav");
    }
    if (ui->OMOptimizedRadio->isChecked()) {
        s.setValue("output/mode", "optimized");
    } else if (ui->OMSoundtrackRadio->isChecked()) {
        s.setValue("output/mode", "soundtrack");
    }
    s.setValue("output/maxSamplesAfterLoopEnd", ui->OMOOMinSamples->value());
    s.setValue("output/fadeoutStartSec", ui->OMOSFOStartTime->value());
    s.setValue("output/fadeoutLengthSec", ui->OMOSFOLength->value());
    s.setValue("output/loopNumber", ui->OMOSNumLoop->value());

    // close this window
    close();
}

// ChangePageSlotObject

CDChangePageSlotObject::CDChangePageSlotObject(QObject* parent,
                                               QStackedWidget* targetSW,
                                               int targetPage)
    : QObject(parent), targetSW(targetSW), targetPage(targetPage) {}

CDChangePageSlotObject::~CDChangePageSlotObject() {}

void CDChangePageSlotObject::changePage() {
    targetSW->setCurrentIndex(targetPage);
    qDebug().noquote() << QString::asprintf("change page: %d", targetPage);
}
