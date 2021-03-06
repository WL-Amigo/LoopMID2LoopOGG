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
    bool saveWithLoop(QFile& wavefile, quint32 loopStart, quint32 loopLength,
                      quint32 loopNum);
    quint32 getOverlap(quint32 offsetSample);
    void mixAt(RIFFWaveEditor& source, quint32 offsetSample);
    quint32 getLengthInSample();
    void cutoutAfter(quint32 offsetSample);
    void expFadeout(quint32 offsetSample, quint32 fadeLength);
    void safeExpFadeoutWithLoop(quint32 offsetSample, quint32 fadeLength,
                                quint32 loopStart, quint32 loopLength);
    void linearFadeout(quint32 offsetSample, quint32 fadeLength);
    void safeLinearFadeoutWithLoop(quint32 offsetSample, quint32 fadeLength,
                                   quint32 loopStart, quint32 loopLength);
    void amplify(int ampPercentage);

private:  // methods
    bool checkApplicability(const QByteArray& formatChunk);
    void loadWaveForm(const QByteArray& dataChunk);
    void serializeWaveForm(QByteArray& dataChunkDest);
    bool saveCommonData(QFile& wavefile, quint32 waveformSize);
    quint32 getSampleRate(const QByteArray& formatChunk);
    float getMaxAmplitude();
    void multiplyAmp(double amp);
    void compressAllByMaxAmplitude();
    void compressDynamicallyByMaxAmplitude(qreal ratio);
    void prepareSafeFadeoutWithLoop(quint32 offsetSample, quint32 fadeLength,
                                    quint32 loopStart, quint32 loopLength);

private:  // variable
    QVector<float> rChannelFloat;
    QVector<float> lChannelFloat;
    quint32 sampleRate;
};

#endif  // RIFFWAVEEDITOR_H
