#include "includes/ConfigDialog.hpp"
#include "ui_ConfigDialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QMetaEnum>
#include <QPushButton>
#include <QSettings>

#include "QAACEncoderExecutor.hpp"
#include "includes/EncoderExecutorProvider.hpp"

#include "GlobalConstants.hpp"

ConfigDialog::ConfigDialog(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ConfigDialog),
      m_HowToEnableAACTips(new QMessageBox(QMessageBox::NoIcon,
                                           tr("How to enable AAC?"), "",
                                           QMessageBox::Ok, this)) {
    ui->setupUi(this);

    // setup config selector buttons
    setupConfigSelectorButtons();
    ui->pagesWidget->setCurrentIndex(0);

    // setup file browse line edits
    this->setupFileBrowseLineEdits();

    // connect behavior
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &ConfigDialog::saveSettingsAndClose);

    // restore current settings
    restoreSettingsToUI();

    // set tips
    this->m_HowToEnableAACTips->setTextFormat(Qt::RichText);
    this->m_HowToEnableAACTips->setText(
        tr("This application needs latest "
           "iTunes or QuickTime installation"
           "for encoding to AAC file.<br/>"
           "You can get iTunes <a "
           "href=\"https://www.apple.com/itunes/\">here</a>."));
    connect(ui->HowToEnableAACTipsButtonO, &QPushButton::clicked,
            this->m_HowToEnableAACTips, &QMessageBox::show);
    connect(ui->HowToEnableAACTipsButtonE, &QPushButton::clicked,
            this->m_HowToEnableAACTips, &QMessageBox::show);

    // set platform specific behavior
    setPlatformSpecificBehavior();
}

ConfigDialog::~ConfigDialog() { delete ui; delete m_HowToEnableAACTips; }

void ConfigDialog::setupConfigSelectorButtons() {
    // connect each config selector button to page
    this->connectCSBToPage(ui->CSBOutput, ui->CPOutput);
    this->connectCSBToPage(ui->CSBTiMidity, ui->CPTiMidity);
    this->connectCSBToPage(ui->CSBEffect, ui->CPEffect);
    this->connectCSBToPage(ui->CSBEncoder, ui->CPEncoder);
    this->connectCSBToPage(ui->CSBAbout, ui->CPAbout);
}

void ConfigDialog::setupFileBrowseLineEdits() {
    // connect each correspond line edit and push button
    auto fblec =
        new FileBrowseLineEditConnector(this, ui->TPConfigFileBrowseButton,
                                        ui->TPConfigFileLineEdit, false, false);
    fblec->setAcceptFileTypes(tr("TiMidity++ config file (*.cfg)"));
    fblec =
        new FileBrowseLineEditConnector(this, ui->TPSFDirectoryBrowseButton,
                                        ui->TPSFDirectoryLineEdit, true, false);
}

void ConfigDialog::connectCSBToPage(QToolButton* toolButton,
                                    QWidget* targetPageWidget) {
    // connect config selector button to page specified
    CDChangePageSlotObject* cpso = new CDChangePageSlotObject(
        this, ui->pagesWidget, ui->pagesWidget->indexOf(targetPageWidget));
    connect(toolButton, &QToolButton::clicked, cpso,
            &CDChangePageSlotObject::changePage);
    connect(this, &ConfigDialog::destroyed, cpso,
            &CDChangePageSlotObject::deleteLater);
}

void ConfigDialog::restoreSettingsToUI() {
    // restore all settings to correspond UI elements
    QSettings s;
    QMetaEnum me;

    // --- output section
    QString temp = s.value(ConfigKey::Output::FileType).toString();
    me = QMetaEnum::fromType<ConfigEnums::Output::FileType>();
    if (temp ==
        me.valueToKey(static_cast<int>(ConfigEnums::Output::FileType::ogg))) {
        ui->OFFOggRadio->setChecked(true);
    } else if (temp ==
               me.valueToKey(
                   static_cast<int>(ConfigEnums::Output::FileType::aac))) {
        ui->OFFAACRadio->setChecked(true);
    } else if (temp ==
               me.valueToKey(static_cast<int>(
                   ConfigEnums::Output::FileType::oggAndAac))) {
        ui->OFFOggAndAACRadio->setChecked(true);
    } else if (temp ==
               me.valueToKey(
                   static_cast<int>(ConfigEnums::Output::FileType::wav))) {
        ui->OFFWaveRadio->setChecked(true);
    }
    temp = s.value(ConfigKey::Output::Mode).toString();
    me = QMetaEnum::fromType<ConfigEnums::Output::Mode>();
    if (temp ==
        me.valueToKey(static_cast<int>(ConfigEnums::Output::Mode::optimized))) {
        ui->OMOptimizedRadio->setChecked(true);
    } else if (temp ==
               me.valueToKey(
                   static_cast<int>(ConfigEnums::Output::Mode::soundtrack))) {
        ui->OMSoundtrackRadio->setChecked(true);
    }
    ui->OMOOMinSamples->setValue(
        s.value(ConfigKey::Output::MaxSamplesAfterLoopEnd).toInt());
    ui->OMOSFOStartTime->setValue(
        s.value(ConfigKey::Output::FadeoutStartSec).toDouble());
    ui->OMOSFOLength->setValue(
        s.value(ConfigKey::Output::FadeoutLengthSec).toDouble());
    ui->OMOSNumLoop->setValue(s.value(ConfigKey::Output::LoopNumber).toInt());

    // --- TiMidity++ section
    ui->TPConfigFileLineEdit->setText(
        s.value(ConfigKey::TiMidity::ConfigFilePath).toString());
    ui->TPSFDirectoryLineEdit->setText(
        s.value(ConfigKey::TiMidity::SoundfontDirPath).toString());

    // --- Effect section
    me = QMetaEnum::fromType<ConfigEnums::Effect::ReverbModeEnum>();
    auto reverbMode =
        static_cast<ConfigEnums::Effect::ReverbModeEnum>(me.keyToValue(
            s.value(ConfigKey::Effect::ReverbMode).toString().toUtf8().data()));
    if (reverbMode == ConfigEnums::Effect::ReverbModeEnum::disenable) {
        ui->FxRDisenableRadio->setChecked(true);
    } else if (reverbMode == ConfigEnums::Effect::ReverbModeEnum::enable) {
        ui->FxREnableRadio->setChecked(true);
    } else if (reverbMode ==
               ConfigEnums::Effect::ReverbModeEnum::enableGlobal) {
        ui->FxREnableGlobalRadio->setChecked(true);
    }
    ui->FxRLevelSpinBox->setValue(
        s.value(ConfigKey::Effect::ReverbLevel).toInt());

    // -- Encoder section
    me = QMetaEnum::fromType<ConfigEnums::Encoder::OVQualityModeEnum>();
    auto ovQualityMode = static_cast<ConfigEnums::Encoder::OVQualityModeEnum>(
        me.keyToValue(s.value(ConfigKey::Encoder::OggVorbisQualityMode)
                          .toString()
                          .toUtf8()
                          .data()));
    if (ovQualityMode == ConfigEnums::Encoder::OVQualityModeEnum::normal) {
        ui->EncOVQNormalRadio->setChecked(true);
    } else if (ovQualityMode ==
               ConfigEnums::Encoder::OVQualityModeEnum::priorSize) {
        ui->EncOVQPriorSizeRadio->setChecked(true);
    } else if (ovQualityMode ==
               ConfigEnums::Encoder::OVQualityModeEnum::custom) {
        ui->EncOVQCustomRadio->setChecked(true);
    }
    ui->EncOVQValueSpinBox->setValue(
        s.value(ConfigKey::Encoder::OggVorbisQualityValue).toInt());
    me = QMetaEnum::fromType<ConfigEnums::Encoder::QAACQualityModeEnum>();
    auto qaacQualityMode =
        static_cast<ConfigEnums::Encoder::QAACQualityModeEnum>(
            me.keyToValue(s.value(ConfigKey::Encoder::QAACQualityMode)
                              .toString()
                              .toUtf8()
                              .data()));
    if (qaacQualityMode == ConfigEnums::Encoder::QAACQualityModeEnum::normal) {
        ui->EncQAACQNormalRadio->setChecked(true);
    } else if (qaacQualityMode ==
               ConfigEnums::Encoder::QAACQualityModeEnum::priorSize) {
        ui->EncQAACQPriorSizeRadio->setChecked(true);
    } else if (qaacQualityMode ==
               ConfigEnums::Encoder::QAACQualityModeEnum::custom) {
        ui->EncQAACQCustomRadio->setChecked(true);
    }
    ui->EncQAACABRSpinBox->setValue(
        s.value(ConfigKey::Encoder::QAACAverageBitRate).toInt());
}

void ConfigDialog::saveSettingsAndClose() {
    // save all settings
    QSettings s;
    QMetaEnum me;
    // --- output section
    me = QMetaEnum::fromType<ConfigEnums::Output::FileType>();
    if (ui->OFFOggRadio->isChecked()) {
        s.setValue(ConfigKey::Output::FileType,
                   me.valueToKey(
                       static_cast<int>(ConfigEnums::Output::FileType::ogg)));
    } else if (ui->OFFAACRadio->isChecked()) {
        s.setValue(ConfigKey::Output::FileType,
                   me.valueToKey(
                       static_cast<int>(ConfigEnums::Output::FileType::aac)));
    } else if (ui->OFFOggAndAACRadio->isChecked()) {
        s.setValue(ConfigKey::Output::FileType,
                   me.valueToKey(static_cast<int>(
                       ConfigEnums::Output::FileType::oggAndAac)));
    } else if (ui->OFFWaveRadio->isChecked()) {
        s.setValue(ConfigKey::Output::FileType,
                   me.valueToKey(
                       static_cast<int>(ConfigEnums::Output::FileType::wav)));
    }
    me = QMetaEnum::fromType<ConfigEnums::Output::Mode>();
    if (ui->OMOptimizedRadio->isChecked()) {
        s.setValue(ConfigKey::Output::Mode,
                   me.valueToKey(
                       static_cast<int>(ConfigEnums::Output::Mode::optimized)));
    } else if (ui->OMSoundtrackRadio->isChecked()) {
        s.setValue(ConfigKey::Output::Mode,
                   me.valueToKey(static_cast<int>(
                       ConfigEnums::Output::Mode::soundtrack)));
    }
    s.setValue(ConfigKey::Output::MaxSamplesAfterLoopEnd,
               ui->OMOOMinSamples->value());
    s.setValue(ConfigKey::Output::FadeoutStartSec,
               ui->OMOSFOStartTime->value());
    s.setValue(ConfigKey::Output::FadeoutLengthSec, ui->OMOSFOLength->value());
    s.setValue(ConfigKey::Output::LoopNumber, ui->OMOSNumLoop->value());

    // --- TiMidity++ section
    s.setValue(ConfigKey::TiMidity::ConfigFilePath,
               ui->TPConfigFileLineEdit->text());
    s.setValue(ConfigKey::TiMidity::SoundfontDirPath,
               ui->TPSFDirectoryLineEdit->text());

    // --- Effect section
    me = QMetaEnum::fromType<ConfigEnums::Effect::ReverbModeEnum>();
    ConfigEnums::Effect::ReverbModeEnum reverbMode;
    if (ui->FxRDisenableRadio->isChecked()) {
        reverbMode = ConfigEnums::Effect::ReverbModeEnum::disenable;
    } else if (ui->FxREnableRadio->isChecked()) {
        reverbMode = ConfigEnums::Effect::ReverbModeEnum::enable;
    } else if (ui->FxREnableGlobalRadio->isChecked()) {
        reverbMode = ConfigEnums::Effect::ReverbModeEnum::enableGlobal;
    }
    s.setValue(ConfigKey::Effect::ReverbMode,
               me.valueToKey(static_cast<int>(reverbMode)));
    s.setValue(ConfigKey::Effect::ReverbLevel, ui->FxRLevelSpinBox->value());

    // --- Encoder section
    me = QMetaEnum::fromType<ConfigEnums::Encoder::OVQualityModeEnum>();
    ConfigEnums::Encoder::OVQualityModeEnum ovQualityMode;
    if (ui->EncOVQNormalRadio->isChecked()) {
        ovQualityMode = ConfigEnums::Encoder::OVQualityModeEnum::normal;
    } else if (ui->EncOVQPriorSizeRadio->isChecked()) {
        ovQualityMode = ConfigEnums::Encoder::OVQualityModeEnum::priorSize;
    } else if (ui->EncOVQCustomRadio->isChecked()) {
        ovQualityMode = ConfigEnums::Encoder::OVQualityModeEnum::custom;
    }
    s.setValue(ConfigKey::Encoder::OggVorbisQualityMode,
               me.valueToKey(static_cast<int>(ovQualityMode)));
    s.setValue(ConfigKey::Encoder::OggVorbisQualityValue,
               ui->EncOVQValueSpinBox->value());
    me = QMetaEnum::fromType<ConfigEnums::Encoder::QAACQualityModeEnum>();
    ConfigEnums::Encoder::QAACQualityModeEnum qaacQualityMode;
    if (ui->EncQAACQNormalRadio->isChecked()) {
        qaacQualityMode = ConfigEnums::Encoder::QAACQualityModeEnum::normal;
    } else if (ui->EncQAACQPriorSizeRadio->isChecked()) {
        qaacQualityMode = ConfigEnums::Encoder::QAACQualityModeEnum::priorSize;
    } else if (ui->EncQAACQCustomRadio->isChecked()) {
        qaacQualityMode = ConfigEnums::Encoder::QAACQualityModeEnum::custom;
    }
    s.setValue(ConfigKey::Encoder::QAACQualityMode,
               me.valueToKey(static_cast<int>(qaacQualityMode)));
    s.setValue(ConfigKey::Encoder::QAACAverageBitRate,
               ui->EncQAACABRSpinBox->value());

    // close this window
    close();
}

void ConfigDialog::setPlatformSpecificBehavior() {
    // for Windows
#ifdef Q_OS_WIN
    // check qaac availability
    auto qaacEE = EncoderExecutorProvider::getQAAC();
    if(qaacEE.isNull()){
        // if qaac not available, disenable aac related settings
        ui->OFFAACRadio->setEnabled(false);
        ui->OFFOggAndAACRadio->setEnabled(false);
        ui->EncQAACQualityGroupBox->setEnabled(false);
    }
#endif

// for Linux
#ifdef Q_OS_LINUX
    // disenable AAC related settings
    ui->OFFAACRadio->hide();
    ui->OFFOggAndAACRadio->hide();
    ui->EncQAACQualityGroupBox->hide();
    ui->HowToEnableAACTipsButtonE->hide();
    ui->HowToEnableAACTipsButtonO->hide();
#endif
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

// FileBrowseLineEditConnector

FileBrowseLineEditConnector::FileBrowseLineEditConnector(
    QWidget* parent, QPushButton* browseButton, QLineEdit* lineEdit,
    bool directory, bool absolute)
    : QObject(parent),
      m_parent(parent),
      m_absolute(absolute),
      m_directory(directory),
      m_lineEdit(lineEdit),
      m_acceptFileTypes("") {
    connect(browseButton, &QPushButton::clicked, this,
            &FileBrowseLineEditConnector::browseFile);
}

FileBrowseLineEditConnector::~FileBrowseLineEditConnector() {}

void FileBrowseLineEditConnector::setAcceptFileTypes(
    const QString& fileTypesString) {
    this->m_acceptFileTypes = fileTypesString;
}

void FileBrowseLineEditConnector::browseFile() {
    QString path;
    if (this->m_directory) {
        path = QFileDialog::getExistingDirectory(this->m_parent,
                                                 tr("Select directory..."));

    } else {
        path =
            QFileDialog::getOpenFileName(this->m_parent, tr("Select file..."),
                                         QString(), this->m_acceptFileTypes);
    }
    if (path.isEmpty()) return;

    if (!this->m_absolute) {
        // calcurate relative path
        QDir applicationDir(QCoreApplication::applicationDirPath());
        path = applicationDir.relativeFilePath(path);
    }
    this->m_lineEdit->setText(path);
}
