#ifndef OGGVORBISENCODEREXECUTOR_HPP
#define OGGVORBISENCODEREXECUTOR_HPP

#include "EncoderExecutor.hpp"

class OggVorbisEncoderExecutor: public EncoderExecutor
{
public:
    OggVorbisEncoderExecutor();
    int execute(QString inputFilePath, QString outputFilePath);
};

#endif // OGGVORBISENCODEREXECUTOR_HPP
