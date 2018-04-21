#include "QAACEncoderExecutor.hpp"

#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QtMath>

bool QAACEncoderExecutor::isQAACAvailable() {
    // check qaac availability

    // spawn qaac with --check flag
    QProcess qaacProc;
    qaacProc.start(this->m_binaryPath, {"--check"});
    if (!qaacProc.waitForFinished()) {
        return false;
    }

    // check output string includes "CoreAudioToolbox"
    // which indicates AAS is installed
    QString output = QString::fromUtf8(qaacProc.readAllStandardError());
    return output.contains("CoreAudioToolbox");
}

QAACEncoderExecutor::QAACEncoderExecutor() {
    this->m_qualityValue = 128.0;  // Average Bit Rate [kbps]
}

int QAACEncoderExecutor::execute(QString inputFilePath, QString outputFilePath,
                                 qint32 loopStart, qint32 loopLength) {
    // build command line
    QStringList command;
    if (loopStart >= 0 && loopLength > 0) {
        command << "--long-tag" << QString::asprintf("LOOPSTART:%d", loopStart);
        command << "--long-tag"
                << QString::asprintf("LOOPLENGTH:%d", loopLength);
    }
    command << "-a" << QString::asprintf("%d", qRound(this->m_qualityValue));

    // if ABR <= 64, enable HE AAC mode
    if (qRound(this->m_qualityValue) <= 64) {
        command << "--he";
    }

    command << "-o" << outputFilePath + ".m4a";
    command << inputFilePath;

    qDebug().noquote() << command;
    // spawn qaac encoder
    return QProcess::execute(this->m_binaryPath, command);
}
