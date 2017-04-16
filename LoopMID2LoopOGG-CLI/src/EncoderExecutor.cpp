#include "EncoderExecutor.hpp"

EncoderExecutor::EncoderExecutor():
    m_qualityValue(0.0f), // should set in derived class constructor
    m_loopStart(-1),
    m_loopLength(-1),
    m_binaryPath("") // should set in derived class constructor
{}
void EncoderExecutor::setQualityValue(qreal value){
    this->m_qualityValue = value;
}
void EncoderExecutor::setLoopStart(qint32 sample){
    this->m_loopStart = sample;
}
void EncoderExecutor::setLoopLength(qint32 sample){
    this->m_loopLength = sample;
}
void EncoderExecutor::setBinaryPath(QString binaryPath){
    this->m_binaryPath = binaryPath;
}
