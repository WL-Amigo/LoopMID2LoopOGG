#include "EncoderExecutor.hpp"

EncoderExecutor::EncoderExecutor():
    m_qualityValue(0.0f), // should set in derived class constructor
    m_binaryPath("") // should set in derived class constructor
{}
void EncoderExecutor::setQualityValue(qreal value){
    this->m_qualityValue = value;
}
void EncoderExecutor::setBinaryPath(QString binaryPath){
    this->m_binaryPath = binaryPath;
}
