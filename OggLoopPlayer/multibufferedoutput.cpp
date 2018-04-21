#include "multibufferedoutput.hpp"

MultiBufferedOutput::MultiBufferedOutput(QObject* parent = 0)
    : QIODevice(parent) {
    cullentBufferPosition = 0;
}

MultiBufferedOutput::~MultiBufferedOutput() { bufferQueue.clear(); }

void MultiBufferedOutput::start() {
    if (!open(QIODevice::ReadOnly)) {
        qDebug() << Q_FUNC_INFO << " : opening device failed";
    }
}

void MultiBufferedOutput::stop() {
    close();
    // prepare for next start()
    bufferQueue.clear();
    cullentBuffer.reset();
    cullentBufferPosition = 0;
}

qint64 MultiBufferedOutput::readData(char* data, qint64 maxLength) {
    if (cullentBuffer.data() == nullptr) {
        if (bufferQueue.size() > 0) {
            cullentBuffer = bufferQueue.dequeue();
        } else {
            qDebug() << Q_FUNC_INFO << " : there is no data can be read";
            return 0;
        }
    }

    int dataIndex = 0;
    const char* audioData = cullentBuffer->constData<char>();
    while (maxLength > dataIndex) {
        if (cullentBufferPosition == cullentBuffer->byteCount()) {
            queueLock.lock();
            if (bufferQueue.empty()) {
                qDebug() << Q_FUNC_INFO << " : bufferQueue empty";
                cullentBuffer.reset();
                cullentBufferPosition = 0;
                queueLock.unlock();
                break;
            }
            cullentBuffer = bufferQueue.dequeue();
            queueLock.unlock();
            cullentBufferPosition = 0;
            audioData = cullentBuffer->constData<char>();
        }

        // write data to input data pointer from audioData
        data[dataIndex] = audioData[cullentBufferPosition];
        dataIndex++;
        cullentBufferPosition++;
    }

    return dataIndex;
}

qint64 MultiBufferedOutput::writeData(const char* data, qint64 length) {
    return 0;
}

qint64 MultiBufferedOutput::bytesAvailable() const {
    qint64 byte = 0;
    for (auto&& buf : bufferQueue) {
        byte += buf->byteCount();
    }

    return byte - cullentBufferPosition;
}

bool MultiBufferedOutput::queueBuffer(QSharedPointer<QAudioBuffer>& newBuffer) {
    queueLock.lock();

    bufferQueue.enqueue(newBuffer);

    queueLock.unlock();

    return true;
}

int MultiBufferedOutput::getQueuedBufferNum() {
    int queueSize = 0;

    queueLock.lock();

    queueSize = bufferQueue.size();

    queueLock.unlock();

    return queueSize;
}
