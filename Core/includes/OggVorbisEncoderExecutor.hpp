#ifndef OGGVORBISENCODEREXECUTOR_HPP
#define OGGVORBISENCODEREXECUTOR_HPP

#include "EncoderExecutor.hpp"

class OggVorbisEncoderExecutor: public EncoderExecutor
{
public:
    OggVorbisEncoderExecutor();
    virtual ~OggVorbisEncoderExecutor(){}
    virtual int execute(QString inputFilePath, QString outputFilePath, qint32 loopStart = -1, qint32 loopLength = -1);
};

#endif // OGGVORBISENCODEREXECUTOR_HPP
