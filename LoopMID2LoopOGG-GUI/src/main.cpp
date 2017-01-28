#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
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

static void initializeSettings() {
    // initialize settings on first execution of application
    QSettings s;
    s.setValue("outputDirectory", "");
    s.setValue("preview", false);

    // output configuration
    s.setValue("output/fileType", "ogg");    // ogg, wav
    s.setValue("output/mode", "optimized");  // optimized, soundtrack
    s.setValue("output/maxSamplesAfterLoopEnd", 22050);
    s.setValue("output/fadeoutStartSec", 2.0f);
    s.setValue("output/fadeoutLengthSec", 6.0f);
    s.setValue("output/loopNumber", 2);
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
    if (s.value("reset", true).toBool()) {
        initializeSettings();
        s.setValue("reset", false);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
