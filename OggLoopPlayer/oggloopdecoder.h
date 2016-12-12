#ifndef OGGLOOPDECODER_H
#define OGGLOOPDECODER_H

#include <QObject>

#include <QString>
#include <QAudioBuffer>
#include <QAudioFormat>
#include <QSharedPointer>

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "stdio.h"

class OggLoopDecoder : public QObject
{
	Q_OBJECT

public:
    static const qint64 defaultDecodeLength = 1024 * 16;

private:
    FILE* oggFile;
	OggVorbis_File oggStream;
    vorbis_comment* vorbisComment;
    vorbis_info* vorbisInfo;

    QAudioFormat format;
    ogg_int64_t loopStart;
    ogg_int64_t loopEnd;
    bool opened;
    bool loopEnable;
    int endian; // 0 : Little endian, 1 : Big endian

    QString errorReason;

public:
	OggLoopDecoder(QObject* parent);

    bool open(QString& fileName);
    bool open(const char* fileName);
    bool isOpen();
    void close();
    QAudioFormat getAudioFormat();
    QSharedPointer<QAudioBuffer> decode(qint64 length = 4096 * 4);
    bool seekTo(size_t sample);
    void setLoop(bool newLoopEnable);
    void setEndianness(int newEndian);
	qint64 getCullentPositionAbout();
	qint64 getAllSampleNum();
	QString getTitle();

    QString getErrorReason();

signals:
	void looped(qint64 rewindedPosition);

protected:
    QString getOggErrorString(int code);


};

#endif // OGGLOOPDECODER_H
