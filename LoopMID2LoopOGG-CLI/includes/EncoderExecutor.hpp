#ifndef ENCODEREXECUTOR_HPP
#define ENCODEREXECUTOR_HPP

#include <QtCore>

class EncoderExecutor
{
protected:
    qreal m_qualityValue;
public:
    EncoderExecutor();
    void setQualityValue(qreal value);
    virtual int execute();
};

#endif // ENCODEREXECUTOR_HPP
