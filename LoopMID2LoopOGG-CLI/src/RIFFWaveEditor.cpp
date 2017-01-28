#include "RIFFWaveEditor.h"

#include <QDebug>
#include <QIODevice>
#include <QtMath>

RIFFWaveEditor::RIFFWaveEditor() {
    this->rChannelFloat = QVector<float>();
    this->lChannelFloat = QVector<float>();
}

quint32 getUInt32FromByteArray(const QByteArray &data, bool bigEndian = false) {
    Q_ASSERT(data.size() == 4);
    quint32 ret = 0;
    const char *dataPtr = data.data();
    ret = *(reinterpret_cast<const quint32 *>(dataPtr));
    return ret;
}

quint16 getUInt16FromByteArray(const QByteArray &data, bool bigEndian = false) {
    Q_ASSERT(data.size() == 2);
    quint16 ret = 0;
    const char *dataPtr = data.data();
    ret = *(reinterpret_cast<const quint16 *>(dataPtr));
    return ret;
}

qint16 getInt16FromByteArray(const QByteArray &data, bool bigEndian = false) {
    Q_ASSERT(data.size() == 2);
    qint16 ret = 0;
    const char *dataPtr = data.data();
    ret = *(reinterpret_cast<const qint16 *>(dataPtr));
    return ret;
}

bool RIFFWaveEditor::open(QFile &wavefile) {
    // open file
    if (!wavefile.open(QIODevice::ReadOnly)) return false;

    // read all as byte array
    QByteArray data = wavefile.readAll();
    if (data.isNull()) return false;

    // check file header
    if (!(data.mid(0, 4) == "RIFF") || !(data.mid(8, 4) == "WAVE"))
        return false;

    // get file size
    //    quint32 filesize = getUInt32FromByteArray(data.mid(4,4));
    //    qDebug() << "filesize:" << filesize;

    // check applicability of inputed file
    int fmtIndex = data.indexOf("fmt ");
    if (fmtIndex < 0) return false;
    quint32 fmtSize = getUInt32FromByteArray(data.mid(fmtIndex + 4, 4));
    if (!checkApplicability(data.mid(fmtIndex + 8, fmtSize))) return false;
    qDebug() << "check applicability: pass";

    // get sample rate
    this->sampleRate = this->getSampleRate(data.mid(fmtIndex + 8, fmtSize));
    qDebug() << "sample rate:" << this->sampleRate;

    // get wave form data
    int dataIndex = data.indexOf("data");
    if (dataIndex < 0) return false;
    quint32 dataSize = getUInt32FromByteArray(data.mid(dataIndex + 4, 4));
    if (dataSize < 0) return false;
    this->loadWaveForm(data.mid(dataIndex + 8, dataSize));

    //    qDebug() << "wave-form data(L):" << this->lChannel.mid(0,16);
    //    qDebug() << "wave-form data(R):" << this->rChannel.mid(0,16);

    // close file
    if (wavefile.isOpen()) wavefile.close();

    return true;
}

bool RIFFWaveEditor::checkApplicability(const QByteArray &formatChunk) {
    // formatChunk should be content of format chunk (without "fmt " and size)
    return formatChunk[0] == 0x01
           && formatChunk[1] == 0x00  // it should be Linear PCM Format
           && formatChunk[2] == 0x02
           && formatChunk[3] == 0x00  // channel # should be 2
           && (getUInt32FromByteArray(formatChunk.mid(4, 4)) * 4
               == getUInt32FromByteArray(
                      formatChunk.mid(8, 4)))  // validity check
           && formatChunk[12] == 0x04 && formatChunk[13] == 0x00
           && formatChunk[14] == 0x10 && formatChunk[15] == 0x00;
}

quint32 RIFFWaveEditor::getSampleRate(const QByteArray &formatChunk) {
    return getUInt32FromByteArray(formatChunk.mid(4, 4));
}

void RIFFWaveEditor::loadWaveForm(const QByteArray &dataChunk) {
    // dataChunk should be wave-form data (without "data" and size)
    quint32 dataSize = dataChunk.size();
    for (quint32 idx = 0; idx < dataSize; idx += 4) {
        this->lChannelFloat.append(
            static_cast<float>(getInt16FromByteArray(dataChunk.mid(idx, 2)))
            / 32768.0f);
        this->rChannelFloat.append(
            static_cast<float>(getInt16FromByteArray(dataChunk.mid(idx + 2, 2)))
            / 32768.0f);
    }
}

bool RIFFWaveEditor::save(QFile &wavefile) {
    // open file with write mode
    if (!wavefile.open(QIODevice::WriteOnly)) return false;

    // compress waveform if necessary
    if (this->getMaxAmplitude() > 1.0f) this->compressAllByMaxAmplitude();
    qDebug() << "max amp after compress:" << this->getMaxAmplitude();

    // create serialized wave-form data
    QByteArray waveformData;
    this->serializeWaveForm(waveformData);
    quint32 waveformDataSize = waveformData.size();

    // calculate filesize
    quint32 fileSize = 12                       // RIFF File Header
                       + 24                     // format chunk
                       + 8 + waveformDataSize;  // data chunk
    quint32 fileSizeWithoutHeader = fileSize - 8;

    // write RIFF header
    wavefile.write("RIFF");
    wavefile.write((reinterpret_cast<const char *>(&fileSizeWithoutHeader)), 4);
    wavefile.write("WAVE");

    // write format chunk
    QByteArray formatChunk;
    quint32 byteRate = this->sampleRate * 4;
    formatChunk.append("fmt ");
    formatChunk.append(1, 0x10);
    formatChunk.append(3, 0x00);
    formatChunk.append(1, 0x01);
    formatChunk.append(1, 0x00);
    formatChunk.append(1, 0x02);
    formatChunk.append(1, 0x00);
    formatChunk.append(reinterpret_cast<const char *>(&(this->sampleRate)), 4);
    formatChunk.append(reinterpret_cast<const char *>(&byteRate), 4);
    formatChunk.append(1, 0x04);
    formatChunk.append(1, 0x00);
    formatChunk.append(1, 0x10);
    formatChunk.append(1, 0x00);
    wavefile.write(formatChunk);

    // write data chunk
    wavefile.write("data");
    wavefile.write(reinterpret_cast<const char *>(&waveformDataSize), 4);
    wavefile.write(waveformData);

    // close file
    wavefile.close();

    return false;
}

void RIFFWaveEditor::serializeWaveForm(QByteArray &dataChunkDest) {
    quint32 dataSize = lChannelFloat.size();
    Q_ASSERT(dataSize = rChannelFloat.size());
    for (quint32 idx = 0; idx < dataSize; idx++) {
        qint16 lSample
            = static_cast<qint16>(qRound(lChannelFloat[idx] * 32767.0f));
        qint16 rSample
            = static_cast<qint16>(qRound(rChannelFloat[idx] * 32767.0f));
        dataChunkDest.append(reinterpret_cast<const char *>(&lSample), 2);
        dataChunkDest.append(reinterpret_cast<const char *>(&rSample), 2);
    }
}

quint32 RIFFWaveEditor::getOverlap(quint32 offsetSample) {
    return lChannelFloat.size() - offsetSample;
}

void RIFFWaveEditor::mixAt(RIFFWaveEditor &source, quint32 offsetSample) {
    // calculate overlap between this and source
    int overlapSample = this->getOverlap(offsetSample);

    // case: source overlaps on this or touches this (overlapSample >= 0)
    if (overlapSample >= 0) {
        // if overlap samples overcede source samples, appending rest of source
        // is not needed
        if (overlapSample < source.lChannelFloat.size()) {
            lChannelFloat.append(source.lChannelFloat.mid(overlapSample));
            rChannelFloat.append(source.rChannelFloat.mid(overlapSample));
        }
        int iterationRange = overlapSample < source.lChannelFloat.size()
                                 ? overlapSample
                                 : source.lChannelFloat.size();
        for (int olidx = 0; olidx < iterationRange; olidx++) {
            lChannelFloat[offsetSample + olidx]
                = lChannelFloat[offsetSample + olidx]
                  + source.lChannelFloat[olidx];
            rChannelFloat[offsetSample + olidx]
                = rChannelFloat[offsetSample + olidx]
                  + source.rChannelFloat[olidx];
        }
    }
    // case: there are brank samples between this and source (overlapSample < 0)
    else {
        VectorVector for (; overlapSample < 0; overlapSample++) {
            lChannelFloat.push_back(0);
            rChannelFloat.push_back(0);
        }
        lChannelFloat.append(source.lChannelFloat);
        rChannelFloat.append(source.rChannelFloat);
    }
}

quint32 RIFFWaveEditor::getLengthInSample() {
    Q_ASSERT(lChannelFloat.size() == rChannelFloat.size());
    return lChannelFloat.size();
}

void RIFFWaveEditor::compressAllByMaxAmplitude() {
    qDebug() << "compressAllByMaxAmplitude(): called";
    // search sample that has max amplitude
    float maxAmp = this->getMaxAmplitude();

    // calculate compress factor
    float factor = 1.0f / maxAmp;

    // multiply all samples by factor
    quint32 len = this->getLengthInSample();
    for (quint32 idx = 0; idx < len; idx++) {
        lChannelFloat[idx] *= factor;
        rChannelFloat[idx] *= factor;
    }

    return;
}

void RIFFWaveEditor::compressDynamicallyByMaxAmplitude(qreal ratio) {
    // TODO
    return;
}

float RIFFWaveEditor::getMaxAmplitude() {
    quint32 len = this->getLengthInSample();
    float max = 0.0f;
    Vector for (quint32 idx = 0; idx < len; idx++) {
        max = (qAbs(this->lChannelFloat[idx]) > max)
                  ? qAbs(this->lChannelFloat[idx])
                  : max;
        max = (qAbs(this->rChannelFloat[idx]) > max)
                  ? qAbs(this->rChannelFloat[idx])
                  : max;
    }
    qDebug() << "max amplitude:" << max;
    return max;
}

void RIFFWaveEditor::cutoutAfter(quint32 offsetSample) {
    lChannelFloat.remove(offsetSample, lChannelFloat.size() - offsetSample);
    rChannelFloat.remove(offsetSample, rChannelFloat.size() - offsetSample);
}

void RIFFWaveEditor::expFadeout(quint32 offsetSample, quint32 fadeLength) {
    // first: cutout after fadeouted
    cutoutAfter(offsetSample + fadeLength);

    // fadeout by exponential curve
    quint32 endSample = offsetSample + fadeLength;
    float fadeLengthFloat = static_cast<float>(fadeLength);
    for (quint32 sidx = offsetSample; sidx < endSample; sidx++) {
        float amp = qPow(
            0.1, static_cast<float>(sidx - offsetSample) / fadeLength * 4.0f);
        lChannelFloat[sidx] *= amp;
        rChannelFloat[sidx] *= amp;
    }
}
