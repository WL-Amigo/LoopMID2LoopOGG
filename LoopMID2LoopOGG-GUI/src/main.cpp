#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QSettings>
#include <QString>
#include <QStyleFactory>
#include <QTranslator>
#include <QVariant>

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

    app.setPalette(palette);
}

static inline void updateSetting(QSettings &s, const char *key,
                                 QVariant defaultValue, bool &reset) {
    return s.setValue(key, reset ? defaultValue : s.value(key, defaultValue));
}

static void updateSettings(bool reset = false) {
    QSettings s;
    // update settings with taking over settings already set by user
    //     or initialize settings on demand of application reset
    // top page configuration
    updateSetting(s, "outputDirectory", "", reset);
    updateSetting(s, "preview", false, reset);

    // output configuration
    updateSetting(s, "output/fileType", "ogg", reset);
    updateSetting(s, "output/mode", "optimized", reset);
    updateSetting(s, "output/maxSamplesAfterLoopEnd", 22050, reset);
    updateSetting(s, "output/fadeoutStartSec", 2.0f, reset);
    updateSetting(s, "output/fadeoutLengthSec", 6.0f, reset);
    updateSetting(s, "output/loopNumber", 2, reset);
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

    QSettings s;
    if (s.value("reset", true).toBool()
        || s.value("lastUsedVersion", "").toString()
               != GlobalConstants::CURRENT_VERSION) {
        updateSettings(s.value("reset", true).toBool());
        s.setValue("reset", false);
        s.setValue("lastUsedVersion", GlobalConstants::CURRENT_VERSION);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
