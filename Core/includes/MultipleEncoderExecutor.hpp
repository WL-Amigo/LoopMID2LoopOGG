#ifndef MULTIPLEENCODEREXECUTOR_HPP
#define MULTIPLEENCODEREXECUTOR_HPP

#include <QList>
#include <QSharedPointer>
#include "EncoderExecutor.hpp"

class MultipleEncoderExecutor : public EncoderExecutor {
public:
    MultipleEncoderExecutor();
    virtual ~MultipleEncoderExecutor() {}
    virtual int execute(QString inputFilePath, QString outputFilePath,
                        qint32 loopStart, qint32 loopLength);
    void addEncoderExecutor(
        QSharedPointer<EncoderExecutor> encoderExecutorSPtr);

private:
    QList<QSharedPointer<EncoderExecutor>> m_eeList;
};

#endif  // MULTIPLEENCODEREXECUTOR_HPP
