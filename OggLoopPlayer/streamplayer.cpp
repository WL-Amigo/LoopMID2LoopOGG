#include "streamplayer.hpp"
#include <QStringBuilder>

StreamPlayer::StreamPlayer(QObject *parent) :
    QObject(parent), oggDecoder(nullptr), outputDevice(nullptr), audioOutput(nullptr),
    errorReason("no error"), resamplingRate(-1)
{
	oggDecoder = new OggLoopDecoder(this);
	outputDevice = new MultiBufferedOutput(this);
	volume = 1.0;
}

StreamPlayer::~StreamPlayer() {

}

// file opening / closing
bool StreamPlayer::open(QString fileName){

	if(isOpen()){
		// if a file is already opened, close before opening new file
		close();
	}

	// opening file as ogg vorbis audio
    if(!oggDecoder->open(fileName.toLocal8Bit().data())){
		qWarning() << Q_FUNC_INFO << " : failed to open \"" << fileName << "\" as ogg file";
        errorReason = QString("failed to open ") + fileName + "as ogg file --- " + oggDecoder->getErrorReason();
		return false;
	}

	// opening audio output device if needed
	if(audioOutput == nullptr || audioOutput->format() != oggDecoder->getAudioFormat()){
		if(audioOutput != nullptr) delete audioOutput;
		QAudioDeviceInfo deviceInfo = QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
		if(!deviceInfo.isFormatSupported(oggDecoder->getAudioFormat())){
			qWarning() << Q_FUNC_INFO << " : default device not support audio format of decoded ogg file";
			qWarning() << "(opened ogg file will be closed)";
            qWarning() << "\t--audio format of decoded ogg file : " << oggDecoder->getAudioFormat();
            errorReason = "default device not support audio format of decoded ogg file --- format : ";
            errorReason += QString("SR = ") + QString::asprintf("%d", oggDecoder->getAudioFormat().sampleRate());
            errorReason += QString(",CC = ") + QString::asprintf("%d", oggDecoder->getAudioFormat().channelCount());
            errorReason += QString(",SS = ") + QString::asprintf("%d", oggDecoder->getAudioFormat().sampleSize());
            QAudioFormat acceptableFormat = deviceInfo.nearestFormat(oggDecoder->getAudioFormat());
            errorReason += QString(", Acceptable is: SR = ") + QString::asprintf("%d", acceptableFormat.sampleRate());
            errorReason += QString(",CC = ") + QString::asprintf("%d", acceptableFormat.channelCount());
            errorReason += QString(",SS = ") + QString::asprintf("%d", acceptableFormat.sampleSize());
			oggDecoder->close();
			return false;
		}
		audioOutput = new QAudioOutput(deviceInfo, oggDecoder->getAudioFormat(), this);
		audioOutput->setNotifyInterval(10);

	}

	// connect signal audioOutput->notify() to slot this->stream()
	// (by this connection, StreamPlayer stream audio data automaticcaly
	//  every audioOutput has played audio 50msec)
	connect(audioOutput, SIGNAL(notify()), this, SLOT(stream()));
	connect(audioOutput, SIGNAL(notify()), this, SLOT(sendNotify()));

	// set volume
	audioOutput->setVolume(volume);

	return true;

}

bool StreamPlayer::close(){

	// stop playing
	stop();

	// close ogg vorbis file
	oggDecoder->close();

	// disconnect signal notify() to slot stream()
	disconnect(audioOutput, SIGNAL(notify()), this, 0);

	return true;

}

// player control
bool StreamPlayer::play(){

	if(!isOpen()){
		return false;
	}

	if(audioOutput->state() == QAudio::SuspendedState){
		// if audioOutput suspended, call resume()
		return resume();
	} else if(audioOutput->state() == QAudio::StoppedState){
		// if audioOutput stopped, open outputDevice and start audioOutput
		streamMain();
		streamMain();
		outputDevice->start();
		audioOutput->start(outputDevice);
	} else if(audioOutput->state() == QAudio::ActiveState){
		// if audioOutput is active, there is nothing to do
	} else if(audioOutput->state() == QAudio::IdleState){
		// if audioOutput is idle, rewind decoder and play again (this will not be proceed)
		oggDecoder->seekTo(0);
		streamMain();
	}

	return true;

}

bool StreamPlayer::stop(){
	
	if(!isOpen()){
		return false;
	}

	if(audioOutput->state() == QAudio::ActiveState || audioOutput->state() == QAudio::SuspendedState){
		// if audioOutput is active or suspended, stop audioOutput and outputDevice
		audioOutput->stop();
		outputDevice->stop();
		// rewind decoder to 0
		if(!oggDecoder->seekTo(0)){
			qWarning() << Q_FUNC_INFO << " : rewinding decoder failed";
		}
	} else if(audioOutput->state() == QAudio::StoppedState){
		// if audioOutput stopped, nothing to do in here
	} else if(audioOutput->state() == QAudio::IdleState){
		// (this should not be proceeded)
	}

	return true;

}

bool StreamPlayer::pause(){

	if(!isOpen()){
		return false;
	}

	if(audioOutput->state() == QAudio::ActiveState){
		// if audioOutput is active, suspend audioOutput
		audioOutput->suspend();
	} else if(audioOutput->state() == QAudio::SuspendedState || audioOutput->state() == QAudio::StoppedState){
		// if audioOutput is suspended or stopped, nothing to do in here
	} else if(audioOutput->state() == QAudio::IdleState){
		// (this should not be proceeded)
	}

	return true;
}

bool StreamPlayer::resume(){

	if(!isOpen()){
		return false;
	}

	if(audioOutput->state() == QAudio::SuspendedState){
		// if audioOutput is suspended, resume audioOutput
		audioOutput->resume();
	} else if(audioOutput->state() == QAudio::ActiveState || audioOutput->state() == QAudio::StoppedState){
		// if audioOutput is active or stopped, nothing to do in here
	} else if(audioOutput->state() == QAudio::IdleState){
		// (this should not be proceeded)
	}

	return true;

}

bool StreamPlayer::seekTo(qint64 newPosition){

	if(!isOpen()){
		return false;
	}

	QAudio::State cullentState = audioOutput->state();
	bool retBool = true;

	// if audioOutput is playing, stop playing before seeking
	if(cullentState == QAudio::ActiveState || cullentState == QAudio::SuspendedState){
		//audioOutput->stop();
		outputDevice->stop();
	}

	// set decoder cullent position
	oggDecoder->seekTo(newPosition);

	// stream to queue new buffer
	for(int i = 0;i < 2;i++){
		retBool = streamMain();
	}

	// if audioOutput is playing or suspended, reopen outputDevice and audioOutput
	if(cullentState == QAudio::ActiveState || cullentState == QAudio::SuspendedState){
		outputDevice->start();
		//audioOutput->start(outputDevice);
	}

	// if audioOutput is suspended, suspend immediatery
	if(cullentState == QAudio::SuspendedState){
		//audioOutput->suspend();
	}

	return retBool;

}

bool StreamPlayer::changeVolume(int value){

	qreal volume = static_cast<qreal>(value);
	volume /= 100.0;
	if(volume < 0.0) volume = 0.0;
	else if(volume > 1.0) volume = 1.0;

	if(audioOutput != nullptr){
		audioOutput->setVolume(volume);
	}

	this->volume = volume;

	return true;

}

void StreamPlayer::setResampling(int targetSampleRate){
    if(targetSampleRate > 0) resamplingRate = targetSampleRate; // enable resampling to target sampling rate
    else resamplingRate = -1; // unset resampling
}

// information
bool StreamPlayer::isOpen(){

	return oggDecoder->isOpen();

}

qint64 StreamPlayer::getCullentPositionAbout(){

	qint64 decoderPosition = oggDecoder->getCullentPositionAbout();

	decoderPosition -= outputDevice->bytesAvailable() / oggDecoder->getAudioFormat().bytesPerFrame();

	return decoderPosition >= 0 ? decoderPosition : 0;

}

qint64 StreamPlayer::getAllSampleNum(){

	return oggDecoder->getAllSampleNum();

}

QString StreamPlayer::getTitle(){

	return oggDecoder->getTitle();

}

QAudioFormat StreamPlayer::getAudioFormat(){

	return oggDecoder->getAudioFormat();

}

QString StreamPlayer::getErrorReason(){
    return errorReason;
}

// private method
bool StreamPlayer::streamMain(){

	if(outputDevice->getQueuedBufferNum() < 4){
        auto buf = oggDecoder->decode(OggLoopDecoder::defaultDecodeLength * 4);
		if(buf.data() == nullptr){
			qDebug() << Q_FUNC_INFO << " : decoder returns null buffer";
			return false;
        } else {
            if(resamplingRate > 0){
                QSharedPointer<QAudioBuffer> afterResampleBuf(new QAudioBuffer());
                resampleBuffer(*buf, *afterResampleBuf);
                outputDevice->queueBuffer(afterResampleBuf);
            } else {
                outputDevice->queueBuffer(buf);
            }
		}
	}

	return true;

}

bool StreamPlayer::resampleBuffer(QAudioBuffer &source, QAudioBuffer &target){
    // initialize resample handler
//    void* rh = resample_open(1, 1.0, 2.0);

    // calcurate resampling factor
//    double resampleFactor = (double)this->resamplingRate / (double)oggDecoder->getAudioFormat().sampleRate();

    // convert

    return true;
}

// slots

void StreamPlayer::stream(){

	if(!streamMain()){
		qDebug() << Q_FUNC_INFO << " : streamMain() return false";
	}

}

void StreamPlayer::sendNotify(){

	emit notify();

}

