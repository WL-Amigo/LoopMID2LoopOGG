#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QMetaEnum>
#include <QSettings>
#include <QString>
#include <QStyleFactory>
#include <QSysInfo>
#include <QTranslator>
#include <QVariant>

#include "Utils.h"
#include "includes/GlobalConstants.hpp"
#include "mainwindow.h"

static void setFusionStyle(QApplication &app) {
    // set fusion style
    //     from awesome gist: https://gist.github.com/Skyrpex/5547015
    app.setStyle(QStyleFactory::create("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);

    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);

    app.setPalette(palette);
}

static inline void updateSetting(QSettings &s, const QString &key,
                                 QVariant defaultValue, bool &reset) {
    return s.setValue(key, reset ? defaultValue : s.value(key, defaultValue));
}

static void updateSettings(bool reset = false) {
    QSettings s;
    QMetaEnum me;
    // update settings with taking over settings already set by user
    //     or initialize settings on demand of application reset
    // top page configuration
    updateSetting(s, ConfigKey::OutputDirectory, "", reset);
    updateSetting(s, ConfigKey::Preview, false, reset);

    // output configuration
    me = QMetaEnum::fromType<ConfigEnums::Output::FileType>();
    updateSetting(
        s, ConfigKey::Output::FileType,
        me.valueToKey(static_cast<int>(ConfigEnums::Output::FileType::ogg)),
        reset);
    me = QMetaEnum::fromType<ConfigEnums::Output::Mode>();
    updateSetting(
        s, ConfigKey::Output::Mode,
        me.valueToKey(static_cast<int>(ConfigEnums::Output::Mode::optimized)),
        reset);
    updateSetting(s, ConfigKey::Output::MaxSamplesAfterLoopEnd, 22050, reset);
    updateSetting(s, ConfigKey::Output::FadeoutStartSec, 2.0f, reset);
    updateSetting(s, ConfigKey::Output::FadeoutLengthSec, 6.0f, reset);
    updateSetting(s, ConfigKey::Output::LoopNumber, 2, reset);

    // TiMidity++ configuration
    QString appDirStr = QCoreApplication::applicationDirPath();
    updateSetting(s, ConfigKey::TiMidity::ConfigFilePath, "sf2/SGM_v2.01.cfg",
                  reset);
    updateSetting(s, ConfigKey::TiMidity::SoundfontDirPath, "sf2/", reset);

    // effect configuration
    me = QMetaEnum::fromType<ConfigEnums::Effect::ReverbModeEnum>();
    updateSetting(s, ConfigKey::Effect::ReverbMode,
                  me.valueToKey(static_cast<int>(
                      ConfigEnums::Effect::ReverbModeEnum::enable)),
                  reset);
    updateSetting(s, ConfigKey::Effect::ReverbLevel, 64, reset);

    // encoder configuration
    me = QMetaEnum::fromType<ConfigEnums::Encoder::OVQualityModeEnum>();
    updateSetting(s, ConfigKey::Encoder::OggVorbisQualityMode,
                  me.valueToKey(static_cast<int>(
                      ConfigEnums::Encoder::OVQualityModeEnum::normal)),
                  reset);
    updateSetting(s, ConfigKey::Encoder::OggVorbisQualityValue, 4, reset);
    me = QMetaEnum::fromType<ConfigEnums::Encoder::QAACQualityModeEnum>();
    updateSetting(s, ConfigKey::Encoder::OggVorbisQualityMode,
                  me.valueToKey(static_cast<int>(
                      ConfigEnums::Encoder::QAACQualityModeEnum::normal)),
                  reset);
    updateSetting(s, ConfigKey::Encoder::QAACAverageBitRate, 128, reset);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WhiteLuckers");
    QCoreApplication::setApplicationName("LoopMID2LoopOGG");

    setFusionStyle(a);

    QTranslator translator;
    qDebug().noquote() << translator.isEmpty();
    translator.load(":/translations/ja");
    qDebug().noquote() << translator.isEmpty();
    a.installTranslator(&translator);
    //    QLocale::setDefault(QLocale::Japanese);
    qDebug().noquote() << QLocale::system().name();
#ifdef Q_OS_WIN
    qDebug().noquote() << "isWindows64(): " << Utils::isWindows64();
#endif

    QSettings s;
    if (s.value("reset", true).toBool() ||
        s.value("lastUsedVersion", "").toString() !=
            GlobalConstants::CURRENT_VERSION) {
        updateSettings(s.value("reset", true).toBool());
        s.setValue("reset", false);
        s.setValue("lastUsedVersion", GlobalConstants::CURRENT_VERSION);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
