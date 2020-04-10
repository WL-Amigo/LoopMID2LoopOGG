#ifndef LOOPOGGGENERATOR_H
#define LOOPOGGGENERATOR_H

#include <QDir>
#include <QString>
#include "MIDIInfoCollector.hpp"
#include "RIFFWaveEditor.h"
#include "midisettings.h"
#include "TiMidityCommandBuilder.hpp"
#include "EncoderExecutor.hpp"

class LoopOGGGenerator {
public:
    LoopOGGGenerator(QString sourceSMFName, QString outputDir,
                     QString timidityXXBinaryPath, TiMidityCommandBuilder configuredTCB,
                     EncoderExecutor* encoderExecutor);
    bool convert();
    static bool convert(QString sourceSMFName, QString outputDir,
                        QString timidityXXBinaryPath,
                        TiMidityCommandBuilder configuredCommandBuilder, EncoderExecutor* encoderExecutor);

private:
    bool analyzeSMF();
    bool splitSMF();
    bool convertSMFToWAV();
    bool generateLoopWAV();
    bool saveWAVWithTailProcess(RIFFWaveEditor &savingWAV);
    bool convertWAVToOGGWithLoopTag();
    bool resaveWAV();
    bool convertAsOneshot();
    QString getFileNameBase();
    QString getFileNameBase(QString outputPath);
    void saveLoopInformation();
    void sweepTemporaryFiles();

private:
    bool needSplit;
    bool isOneshot;
    QFile smf;
    quint32 loopStartOnMIDI;
    quint32 loopLength;
    quint32 loopOffset;
    quint32 loopStart;
    QDir tempFileDest;

    QString outputDir;
    QString timidityXXBinary;
    TiMidityCommandBuilder timidityCommandBuilder;
    EncoderExecutor* m_encoderExecutor;
};

#endif  // LOOPOGGGENERATOR_H
