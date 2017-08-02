#include "includes/ConvertingWorker.hpp"
#include "LoopOGGGenerator.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QSysInfo>
#include <QSharedPointer>

#include "EncoderExecutorProvider.hpp"
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
    auto ee = EncoderExecutorProvider::getCurrent();
    if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
        LoopOGGGenerator::convert(filename, outputDirectory,
                                  appDirStr + "/TiMidity++/timidity.exe", tcb,
                                  ee.data());
    } else {
        LoopOGGGenerator::convert(filename, outputDirectory, "timidity", tcb,
                                  ee.data());
    }
}
