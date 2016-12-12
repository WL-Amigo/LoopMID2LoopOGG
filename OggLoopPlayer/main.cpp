#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("WhiteLuckers");
    QCoreApplication::setApplicationName("OggLoopPlayer");
    MainWindow w;

	w.connect(&w, SIGNAL(quit()), &a, SLOT(quit()));

    w.show();

    return a.exec();
}
