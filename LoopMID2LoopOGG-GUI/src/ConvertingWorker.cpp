#include "includes/ConvertingWorker.hpp"
#include "LoopOGGGenerator.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QSysInfo>

#include "TiMidityCommandBuilderProvider.h"

ConvertingWorker::ConvertingWorker(QObject *parent, QString &filename,
                                   QString &outputDirectory)
    : filename(filename), outputDirectory(outputDirectory), QThread(parent) {}

void ConvertingWorker::run() {
    qDebug() << "worker started";
    qDebug() << "filename:" << filename;
    qDebug() << "output directory:" << outputDirectory;
    QString appDirStr = QCoreApplication::applicationDirPath();
    TiMidityCommandBuilder tcb = TiMidityCommandBuilderProvider::getDefault();
    if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
        LoopOGGGenerator::convert(filename, outputDirectory,
                                  appDirStr + "/TiMidity++/timidity.exe",
                                  tcb,
                                  appDirStr + "/oggenc2.exe");
    } else {
        LoopOGGGenerator::convert(filename, outputDirectory, "timidity",
                                  tcb, "oggenc");
    }
}
