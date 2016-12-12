#include "includes/ConvertingWorker.hpp"
#include "LoopOGGGenerator.hpp"

#include <QDebug>
#include <QCoreApplication>
#include <QSysInfo>

ConvertingWorker::ConvertingWorker(QObject *parent, QString &filename, QString &outputDirectory) :
    filename(filename), outputDirectory(outputDirectory),
    QThread(parent)
{

}

void ConvertingWorker::run(){
    qDebug() << "worker started";
    qDebug() << "filename:" << filename;
    qDebug() << "output directory:" << outputDirectory;
    QString appDirStr = QCoreApplication::applicationDirPath();
    if(QSysInfo::windowsVersion() != QSysInfo::WV_None){
    LoopOGGGenerator::convert(filename, outputDirectory,
                     appDirStr + "/TiMidity++/timidity.exe",
                     appDirStr + "/TiMidity++/sf2/SGM_v2.01.cfg",
                     appDirStr + "/oggenc2.exe");
    } else {
        LoopOGGGenerator::convert(filename, outputDirectory,
                         "timidity",
                         appDirStr + "/sf2/SGM_v2.01.cfg",
                         "oggenc");
    }
}
