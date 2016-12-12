#ifndef MULTIBUFFEREDOUTPUT_HPP
#define MULTIBUFFEREDOUTPUT_HPP

#include <QDebug>
#include <QQueue>
#include <QIODevice>
#include <QSharedPointer>
#include <QAudioBuffer>
#include <QMutex>

class MultiBufferedOutput : public QIODevice
{
    Q_OBJECT

public:
    MultiBufferedOutput(QObject* parent);
    ~MultiBufferedOutput();

    qint64 readData(char* data, qint64 maxLength);
    qint64 writeData(const char* data, qint64 length);
    qint64 bytesAvailable() const;

    void start();
    void stop();

    bool queueBuffer(QSharedPointer<QAudioBuffer>& newBuffer);
    int getQueuedBufferNum();

private:


    // --- member variables ---
private:
    QQueue<QSharedPointer<QAudioBuffer> > bufferQueue;
    qint64 cullentBufferPosition;
    QSharedPointer<QAudioBuffer> cullentBuffer;
    QMutex queueLock;

};

#endif // MULTIBUFFEREDOUTPUT_HPP
