#include "includes/ConfigDialog.hpp"
#include "ui_ConfigDialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSettings>
#include <QMetaEnum>

#include "GlobalConstants.hpp"

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
    QMetaEnum me;

    // --- output section
    QString temp = s.value(ConfigKey::Output::FileType).toString();
    me = QMetaEnum::fromType<ConfigEnums::Output::FileType>();
    if (temp == me.valueToKey(static_cast<int>(ConfigEnums::Output::FileType::ogg))) {
        ui->OFFOggRadio->setChecked(true);
    } else if (temp == me.valueToKey(static_cast<int>(ConfigEnums::Output::FileType::wav))) {
        ui->OFFWaveRadio->setChecked(true);
    }
    temp = s.value(ConfigKey::Output::Mode).toString();
    me = QMetaEnum::fromType<ConfigEnums::Output::Mode>();
    if (temp == me.valueToKey(static_cast<int>(ConfigEnums::Output::Mode::optimized))) {
        ui->OMOptimizedRadio->setChecked(true);
    } else if (temp == me.valueToKey(static_cast<int>(ConfigEnums::Output::Mode::soundtrack))) {
        ui->OMSoundtrackRadio->setChecked(true);
    }
    ui->OMOOMinSamples->setValue(
        s.value(ConfigKey::Output::MaxSamplesAfterLoopEnd).toInt());
    ui->OMOSFOStartTime->setValue(s.value(ConfigKey::Output::FadeoutStartSec).toDouble());
    ui->OMOSFOLength->setValue(s.value(ConfigKey::Output::FadeoutLengthSec).toDouble());
    ui->OMOSNumLoop->setValue(s.value(ConfigKey::Output::LoopNumber).toInt());
}

void ConfigDialog::saveSettingsAndClose() {
    // save all settings
    QSettings s;
    QMetaEnum me;
    // --- output section
    me = QMetaEnum::fromType<ConfigEnums::Output::FileType>();
    if (ui->OFFOggRadio->isChecked()) {
        s.setValue(ConfigKey::Output::FileType, me.valueToKey(static_cast<int>(ConfigEnums::Output::FileType::ogg)));
    } else if (ui->OFFWaveRadio->isChecked()) {
        s.setValue(ConfigKey::Output::FileType, me.valueToKey(static_cast<int>(ConfigEnums::Output::FileType::wav)));
    }
    me = QMetaEnum::fromType<ConfigEnums::Output::Mode>();
    if (ui->OMOptimizedRadio->isChecked()) {
        s.setValue(ConfigKey::Output::Mode, me.valueToKey(static_cast<int>(ConfigEnums::Output::Mode::optimized)));
    } else if (ui->OMSoundtrackRadio->isChecked()) {
        s.setValue(ConfigKey::Output::Mode, me.valueToKey(static_cast<int>(ConfigEnums::Output::Mode::soundtrack)));
    }
    s.setValue(ConfigKey::Output::MaxSamplesAfterLoopEnd, ui->OMOOMinSamples->value());
    s.setValue(ConfigKey::Output::FadeoutStartSec, ui->OMOSFOStartTime->value());
    s.setValue(ConfigKey::Output::FadeoutLengthSec, ui->OMOSFOLength->value());
    s.setValue(ConfigKey::Output::LoopNumber, ui->OMOSNumLoop->value());

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
