﻿#ifndef RIFFWAVEEDITOR_H
#define RIFFWAVEEDITOR_H

#include <QByteArray>
#include <QFile>
#include <QVector>

class RIFFWaveEditor {
public:
    RIFFWaveEditor();
    bool open(QFile& wavefile);
    bool save(QFile& wavefile);
    quint32 getOverlap(quint32 offsetSample);
    void mixAt(RIFFWaveEditor& source, quint32 offsetSample);
    quint32 getLengthInSample();
    void cutoutAfter(quint32 offsetSample);
    void expFadeout(quint32 offsetSample, quint32 fadeLength);

private:  // methods
    bool checkApplicability(const QByteArray& formatChunk);
    void loadWaveForm(const QByteArray& dataChunk);
    void serializeWaveForm(QByteArray& dataChunkDest);
    quint32 getSampleRate(const QByteArray& formatChunk);
    float getMaxAmplitude();
    void compressAllByMaxAmplitude();
    void compressDynamicallyByMaxAmplitude(qreal ratio);

private:  // variable
    QVector<float> rChannelFloat;
    QVector<float> lChannelFloat;
    quint32 sampleRate;
};

#endif  // RIFFWAVEEDITOR_H
