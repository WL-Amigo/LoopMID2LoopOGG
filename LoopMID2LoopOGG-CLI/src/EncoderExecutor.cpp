#include "EncoderExecutor.hpp"

EncoderExecutor::EncoderExecutor(): m_qualityValue(0.0f) // should set in derived class constructor
{}
void EncoderExecutor::setQualityValue(qreal value);
