#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WhiteLuckers");
    QCoreApplication::setApplicationName("LoopMID2LoopOGG");
    MainWindow w;

    w.show();

    return a.exec();
}
