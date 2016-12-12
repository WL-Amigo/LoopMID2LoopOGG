#include "oggloopdecoder.h"

#include <QDebug>
#include <stdio.h>

OggLoopDecoder::OggLoopDecoder(QObject* parent) : QObject(parent)
{

    loopEnable = false;
    opened = false;
    loopStart = -1;
    loopEnd = -1;
    endian = 0;

	oggFile = nullptr;
    vorbisComment = nullptr;
    vorbisInfo = nullptr;

    errorReason = "no error";

    // set 1 sample size to QAudioFormat
    format.setSampleSize(16);

    // set sample type to QAudioFormat
    format.setSampleType(QAudioFormat::SignedInt);

    // set codec (linear PCM) to QAudioFormat
    format.setCodec("audio/pcm");

    // set endianness to QAudioFormat
    format.setByteOrder(QAudioFormat::LittleEndian);

}

bool OggLoopDecoder::open(QString &fileName){

    return open(fileName.toStdString().c_str());

}

bool OggLoopDecoder::open(const char *fileName){

    // if ogg vorbis file already opened, close file in here
    if(opened){
        close();
    }

    // open file
    if(!(oggFile = fopen(fileName, "rb"))){
        qDebug() << Q_FUNC_INFO << " : failed to open " << fileName;
        this->errorReason = QString("failed to open: ") + fileName;
        return false;
    }

    // open file as ogg vorbis audio
	if((ov_open(oggFile, &oggStream, NULL, 0)) < 0){
        qDebug() << Q_FUNC_INFO << " : failed to open as ogg vorbis file";
        this->errorReason = QString("failed to open as ogg vorbis file");
        fclose(oggFile);
        oggFile = nullptr;
        return false;
    }

    // get vorbis information and vorbis comments
	vorbisInfo = ov_info(&oggStream, -1);
	vorbisComment = ov_comment(&oggStream, -1);

    // set channel number to QAudioFormat
    if(vorbisInfo->channels == 1 || vorbisInfo->channels == 2){
        format.setChannelCount(vorbisInfo->channels);
    } else {
        qDebug() << Q_FUNC_INFO << " : this decoder cannot handle audio data with more than 2 channels";
        this->errorReason = QString("this decoder cannot handle audio data with more than 2 channels");
        fclose(oggFile);
        oggFile = nullptr;
        return false;
    }

    // set sample late to QAudioFormat
    format.setSampleRate(vorbisInfo->rate);
    qDebug() << Q_FUNC_INFO << " : sample rate is " << vorbisInfo->rate;

    // get loop range from ogg vorbis user comment
    QString userComment;
    bool result;
    ogg_int64_t loopStartTemp = -1;
    ogg_int64_t loopLength = -1;
    for(int index = 0;index < vorbisComment->comments;index++){
        userComment = vorbisComment->user_comments[index];
        if(userComment.contains("LOOPSTART",Qt::CaseInsensitive)){
            userComment.remove(0,10);
            loopStartTemp = userComment.toLongLong(&result);
            if(!result){
                qDebug() << Q_FUNC_INFO << " : failed to read loop start sample from LOOPSTART tag";
                loopStartTemp = -1;
            }
        } else if(userComment.contains("LOOPLENGTH",Qt::CaseInsensitive)){
            userComment.remove(0,11);
            loopLength = userComment.toLongLong(&result);
            if(!result){
                qDebug() << Q_FUNC_INFO << " : failed to read loop length sample from LOOPLENGTH tag";
                loopLength = -1;
            }
        }
    }

    // if this succeeded to load LOOPSTART and LOOPLENGTH, set loopStart and loopEnd
    if(loopStartTemp != -1 && loopLength != -1){
        loopStart = loopStartTemp;
        loopEnd = loopLength + loopStartTemp;
        loopEnable = true;
        qDebug() << Q_FUNC_INFO << " : succeed to set loop range";
    } else {
        loopEnable = false;
    }


    opened = true;
    return true;

}

bool OggLoopDecoder::isOpen(){
    return opened;
}

void OggLoopDecoder::close(){

    if(!opened){
        return;
    }

    // reset loop settings
    loopStart = -1;
    loopEnd = -1;
    loopEnable = false;

    // reset vorbisInfo and vorbisComment
    // (nothing to do for resetting because vorbisInfo and vorbisComment
    //  is included in OggVorbis_File)

    // close oggStream
	ov_clear(&oggStream);

    // close file
	//fclose(oggFile);
	//--- this is not required because oggFile is closed by ov_clear

    opened = false;

}

QAudioFormat OggLoopDecoder::getAudioFormat(){

    return format;

}

QSharedPointer<QAudioBuffer> OggLoopDecoder::decode(qint64 length){

    if(!opened){
        qDebug() << Q_FUNC_INFO << " : ogg stream not opened";
    }

    QByteArray byteArray = QByteArray(length, 0);

    int readedSize = 0;
    int section;
    int result;
    ogg_int64_t cullentSample;

    while(readedSize < length){
		result = ov_read(&oggStream, byteArray.data() + readedSize, length - readedSize,
                         endian, 2, 1, &section);

        if(result > 0){
            readedSize += result;
        } else if (result < 0){
            qDebug() << Q_FUNC_INFO << " : " << getOggErrorString(result) << "(in ov_read() )";
            return QSharedPointer<QAudioBuffer>(nullptr);
        } else {
            break;
        }

        // looping process
        // (rewind oggStream and adjust readed sample number)
        if (loopEnable) {
					if ((cullentSample = ov_pcm_tell(&oggStream)) >= loopEnd) {
                        readedSize -= (cullentSample - loopEnd) * vorbisInfo->channels * 2;
                        qDebug() << Q_FUNC_INFO << " : " << cullentSample << "," << readedSize;
						result = ov_pcm_seek(&oggStream, loopStart);
                        if (result != 0){
                            qDebug() << Q_FUNC_INFO << getOggErrorString(result) << "(in ov_pcm_seek() )";
                            return QSharedPointer<QAudioBuffer>(nullptr);
                        }
						emit looped(static_cast<qint64>(loopStart));
                    }
                }
    }

    if(readedSize == 0){
        return QSharedPointer<QAudioBuffer>(nullptr);
    }

    return QSharedPointer<QAudioBuffer>(new QAudioBuffer(byteArray, format));

}

bool OggLoopDecoder::seekTo(size_t sample){

	return ov_pcm_seek(&oggStream, sample) == 0 ? true : false;

}

void OggLoopDecoder::setLoop(bool newLoopEnable){

    loopEnable = newLoopEnable;

}

void OggLoopDecoder::setEndianness(int newEndian){

    if(newEndian != 0 && newEndian != 1) return;

    endian = newEndian;
    if(endian == 0){
        format.setByteOrder(QAudioFormat::LittleEndian);
    } else {
        format.setByteOrder(QAudioFormat::BigEndian);
    }

}

qint64 OggLoopDecoder::getCullentPositionAbout(){

	return static_cast<qint64>(ov_pcm_tell(&oggStream));

}

qint64 OggLoopDecoder::getAllSampleNum(){

	return static_cast<qint64>(ov_pcm_total(&oggStream, -1));

}

QString OggLoopDecoder::getTitle(){

	QString retString = "";
	for(int i = 0;i < vorbisComment->comments;i++){
		retString = vorbisComment->user_comments[i];
		if(retString.contains("TITLE",Qt::CaseInsensitive)){
			retString.remove(0,6);
			break;
		} else {
			retString.clear();
		}
	}

	return retString;

}

QString OggLoopDecoder::getErrorReason(){
    return errorReason;
}

// ---- protected functions ----

QString OggLoopDecoder::getOggErrorString(int code){

    switch (code) {
    case OV_EREAD:
        return QString("Reading from media error.");
    case OV_ENOTVORBIS:
        return QString("Not Vorbis data.");
    case OV_EVERSION:
        return QString("Vorbis version mismatch.");
    case OV_EBADHEADER:
		return QString("Invalid Vorbis header.");
    case OV_EFAULT:
        return QString(
                "Internal logic fault (bug or heap/stack corruption.");
    case OV_EINVAL:
        return QString("Invalid argument error.");
    case OV_ENOSEEK:
        return QString("Ogg file isn't seekable.");
    case OV_EBADLINK:
        return QString("Invalid stream section supplied.");

    default:
        return QString("Unknown Ogg error.");
    }

}
