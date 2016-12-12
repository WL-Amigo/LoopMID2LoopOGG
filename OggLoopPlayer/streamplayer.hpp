#ifndef STREAMPLAYER_HPP
#define STREAMPLAYER_HPP

#include <QObject>

#include "multibufferedoutput.hpp"
#include "oggloopdecoder.h"
#include <QAudioOutput>
#include <QAudioFormat>

class StreamPlayer : public QObject
{
    Q_OBJECT

public:
    explicit StreamPlayer(QObject *parent = 0);
	~StreamPlayer();
	// file opening / closing
	bool open(QString fileName);
	bool close();
	// player control
	bool play();
	bool stop();
	bool pause();
	bool resume();
	bool seekTo(qint64 newPosition);
	bool changeVolume(int value);
    void setResampling(int targetSampleRate);
	// information
	bool isOpen();
	qint64 getCullentPositionAbout();
	qint64 getAllSampleNum();
	QString getTitle();
	QAudioFormat getAudioFormat();
    // error reporting
    QString getErrorReason();

private:
	bool streamMain();
    bool resampleBuffer(QAudioBuffer& source, QAudioBuffer& target);

private:
	MultiBufferedOutput* outputDevice;
	OggLoopDecoder* oggDecoder;
	QAudioOutput* audioOutput;
	qreal volume;

    QString errorReason;
    int resamplingRate;

signals:
	void notify();

public slots:
	void stream();

private slots:
	void sendNotify();
};

#endif // STREAMPLAYER_HPP
