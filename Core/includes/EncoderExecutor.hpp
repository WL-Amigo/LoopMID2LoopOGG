#ifndef ENCODEREXECUTOR_HPP
#define ENCODEREXECUTOR_HPP

#include <QtCore>
#include <QString>

class EncoderExecutor
{
protected:
    qreal m_qualityValue;
    QString m_binaryPath;
public:
    EncoderExecutor();
    virtual ~EncoderExecutor(){}
    void setQualityValue(qreal value);
    void setBinaryPath(QString binaryPath);
    virtual int execute(QString inputFilePath, QString outputFilePath, qint32 loopStart = -1, qint32 loopLength = -1){ return -1; }
};

#endif // ENCODEREXECUTOR_HPP
