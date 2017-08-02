#ifndef ENCODEREXECUTORPROVIDER_HPP
#define ENCODEREXECUTORPROVIDER_HPP

#include "EncoderExecutor.hpp"
#include "OggVorbisEncoderExecutor.hpp"
#include "QAACEncoderExecutor.hpp"

#include <QSharedPointer>

class EncoderExecutorProvider
{
public:
    static QSharedPointer<EncoderExecutor> getCurrent();

public:
    static QSharedPointer<OggVorbisEncoderExecutor> getOggVorbis();
    static QSharedPointer<QAACEncoderExecutor> getQAAC();
};

#endif // ENCODEREXECUTORPROVIDER_HPP
