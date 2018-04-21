#include "OggVorbisEncoderExecutor.hpp"

#include <QDebug>
#include <QProcess>
#include <QStringList>

OggVorbisEncoderExecutor::OggVorbisEncoderExecutor() {
    this->m_qualityValue = 4.0f;
}

int OggVorbisEncoderExecutor::execute(QString inputFilePath,
                                      QString outputFilePath, qint32 loopStart,
                                      qint32 loopLength) {
    // build command line
    QStringList command;
    if (loopStart >= 0 && loopLength > 0) {
        command << "-c" << QString::asprintf("LOOPSTART=%d", loopStart);
        command << "-c" << QString::asprintf("LOOPLENGTH=%d", loopLength);
    }
    command << "-Q";  // enable quiet mode
    command << "-q" << QString::asprintf("%f", this->m_qualityValue);
    command << "-o" << outputFilePath + ".ogg";
    command << inputFilePath;

    qDebug().noquote() << command;
    // spawn ogg vorbis encoder
    return QProcess::execute(this->m_binaryPath, command);
}
