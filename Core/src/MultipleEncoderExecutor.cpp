#include "MultipleEncoderExecutor.hpp"

MultipleEncoderExecutor::MultipleEncoderExecutor() : m_eeList() {}

int MultipleEncoderExecutor::execute(QString inputFilePath,
                                     QString outputFilePath, qint32 loopStart,
                                     qint32 loopLength) {
    // execute added encoder executor
    for (int idx = 0; idx < this->m_eeList.size(); idx++) {
        int result = this->m_eeList.at(idx).data()->execute(
            inputFilePath, outputFilePath, loopStart, loopLength);
        if (result != 0) {  // executor returns non-zero exit status
            return result;
        }
    }
    return 0;
}

void MultipleEncoderExecutor::addEncoderExecutor(
    QSharedPointer<EncoderExecutor> encoderExecutorSPtr) {
    this->m_eeList << encoderExecutorSPtr;
}
