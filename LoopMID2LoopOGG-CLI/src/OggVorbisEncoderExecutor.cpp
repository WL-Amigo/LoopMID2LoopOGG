#include "OggVorbisEncoderExecutor.hpp"

#include <QProcess>
#include <QStringList>

OggVorbisEncoderExecutor::OggVorbisEncoderExecutor() {
    this->m_qualityValue = 4.0f;
}

int OggVorbisEncoderExecutor::execute(QString inputFilePath, QString outputFilePath) {
    // build command line
    QStringList command;
    if(this->m_loopStart >= 0 && this->m_loopLength > 0){
        command << "-c" << QString::asprintf("LOOPSTART=%d", this->m_loopStart);
        command << "-c" << QString::asprintf("LOOPLENGTH=%d", this->m_loopLength);
    }
    command << "-Q";  // enable quiet mode
    command << "-q"
            << QString::asprintf("%f", this->m_qualityValue);
    command << "-o" << outputFilePath;
    command << inputFilePath;

    // spawn ogg vorbis encoder
    return QProcess::execute(this->m_binaryPath, command);
}
