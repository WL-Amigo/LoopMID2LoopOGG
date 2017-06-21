#ifndef ENCODEREXECUTORPROVIDER_HPP
#define ENCODEREXECUTORPROVIDER_HPP

#include "EncoderExecutor.hpp"
#include "OggVorbisEncoderExecutor.hpp"

class EncoderExecutorProvider
{
public:
    static OggVorbisEncoderExecutor getOggVorbis();
};

#endif // ENCODEREXECUTORPROVIDER_HPP
