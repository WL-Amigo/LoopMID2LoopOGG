#ifndef LOOPOGGGENERATOR_H
#define LOOPOGGGENERATOR_H

#include <QDir>
#include <QString>
#include "MIDIInfoCollector.hpp"
#include "RIFFWaveEditor.h"
#include "midisettings.h"
#include "TiMidityCommandBuilder.hpp"

class LoopOGGGenerator {
public:
    LoopOGGGenerator(QString sourceSMFName, QString outputDir,
                     QString timidityXXBinaryPath, TiMidityCommandBuilder configuredTCB,
                     QString oggEncBinaryPath);
    bool convert();
    static bool convert(QString sourceSMFName, QString outputDir,
                        QString timidityXXBinaryPath,
                        TiMidityCommandBuilder configuredCommandBuilder, QString oggEncBinaryPath);

private:
    bool analyzeSMF();
    bool splitSMF();
    bool convertSMFToWAV();
    bool generateLoopWAV();
    bool saveWAVWithTailProcess(RIFFWaveEditor &savingWAV);
    bool convertWAVToOGGWithLoopTag();
    bool resaveWAV();
    QString getFileNameBase();
    QString getFileNameBase(QString outputPath);
    void saveLoopInformation();
    void sweepTemporaryFiles();

private:
    bool needSplit;
    QFile smf;
    quint32 loopStartOnMIDI;
    quint32 loopLength;
    quint32 loopOffset;
    quint32 loopStart;
    QDir tempFileDest;

    QString outputDir;
    QString timidityXXBinary;
    TiMidityCommandBuilder timidityCommandBuilder;
    QString oggEncBinary;
};

#endif  // LOOPOGGGENERATOR_H
