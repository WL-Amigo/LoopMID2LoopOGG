#ifndef QAACENCODEREXECUTOR_HPP
#define QAACENCODEREXECUTOR_HPP

#include "EncoderExecutor.hpp"

class QAACEncoderExecutor : public EncoderExecutor {
public:
    QAACEncoderExecutor();
    virtual ~QAACEncoderExecutor() {}
    virtual int execute(QString inputFilePath, QString outputFilePath,
                        qint32 loopStart = -1, qint32 loopLength = -1);
    bool isQAACAvailable();
};

#endif  // QAACENCODEREXECUTOR_HPP
