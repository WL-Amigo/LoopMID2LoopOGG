#ifndef ENCODEREXECUTOR_HPP
#define ENCODEREXECUTOR_HPP

#include <QtCore>
#include <QString>

class EncoderExecutor
{
protected:
    qreal m_qualityValue;
    qint32 m_loopStart;
    qint32 m_loopLength;
    QString m_binaryPath;
public:
    EncoderExecutor();
    void setQualityValue(qreal value);
    void setLoopStart(qint32 sample);
    void setLoopLength(qint32 sample);
    void setBinaryPath(QString binaryPath);
    virtual int execute(QString inputFilePath, QString outputFilePath){ return -1; }
};

#endif // ENCODEREXECUTOR_HPP
