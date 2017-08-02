﻿#include "LoopOGGGenerator.hpp"
#include <qmath.h>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QSettings>
#include <iostream>
#include "LoopMIDIModifier.hpp"

static const QString IntroSMFSuffix = "_i.mid";
static const QString FirstLoopSMFSuffix = "_l1.mid";
static const QString AfterLoopSMFSuffix = "_l2.mid";
static const QString IntroWAVSuffix = "_i.wav";
static const QString FirstLoopWAVSuffix = "_l1.wav";
static const QString AfterLoopWAVSuffix = "_l2.wav";
static const QString CompleteLoopWAVSuffix = "_lc.wav";
static const QString LoopInformationSuffix = "_loopinfo.json";

static const quint32 SampleRate = 44100;

LoopOGGGenerator::LoopOGGGenerator(QString sourceSMFName, QString outputDir,
                                   QString timidityXXBinaryPath,
                                   TiMidityCommandBuilder configuredTCB,
                                   EncoderExecutor* encoderExecutor)
    : needSplit(false),
      smf(sourceSMFName),
      loopStartOnMIDI(0),
      loopLength(0),
      loopOffset(0),
      loopStart(0),
      tempFileDest(QCoreApplication::applicationDirPath() + "/tmp/"),
      timidityXXBinary(timidityXXBinaryPath),
      m_encoderExecutor(encoderExecutor),
      timidityCommandBuilder(configuredTCB),
      outputDir(outputDir) {}

bool LoopOGGGenerator::convert(QString sourceSMFName, QString outputDir,
                               QString timidityXXBinaryPath,
                               TiMidityCommandBuilder configuredCommandBuilder,
                               EncoderExecutor* encoderExecutor) {
    LoopOGGGenerator log(sourceSMFName, outputDir, timidityXXBinaryPath,
                         configuredCommandBuilder, encoderExecutor);
    return log.convert();
}

bool LoopOGGGenerator::convert() {
    QSettings s;
    QString outputFileType = s.value("output/fileType").toString();
    // run converting process sequencially
    qDebug() << smf.fileName();
    if (!this->analyzeSMF()) return false;
    if (this->needSplit) {
        if (!this->splitSMF()) return false;
    }
    if (!this->convertSMFToWAV()) return false;
    if (!this->generateLoopWAV()) return false;
    if (outputFileType == "wav") {
        if (!this->resaveWAV()) return false;
    } else {
        if (!this->convertWAVToOGGWithLoopTag()) return false;
    }
    this->saveLoopInformation();
    this->sweepTemporaryFiles();

    qDebug() << "loopStartOnMIDI:" << this->loopStartOnMIDI;
    qDebug() << "true loopStart:" << this->loopStart;
    qDebug() << "loopLength:" << this->loopLength;
    qDebug() << "needSplit:" << this->needSplit;
    qDebug() << "loopOffset:" << this->loopOffset;

    return true;
}

bool LoopOGGGenerator::analyzeSMF() {
    //    std::string smfname = smf.fileName().toUtf8().toStdString();
    std::string smfname = QFile::encodeName(smf.fileName()).toStdString();
    qDebug() << QString::fromStdString(smfname);
    MIDIInfoCollector mic(smfname);

    // get LOOPSTART/LOOPLENGTH on MIDI data
    this->loopStartOnMIDI = mic.getLoopStartSample(SampleRate);
    this->loopLength = mic.getLoopLengthSample(SampleRate);

    // get nesessality of splitting SMF
    this->needSplit = mic.hasIntro();

    // get offset of appending loop sample
    this->loopOffset = mic.getLoopAppendOffsetSample(SampleRate);

    return true;
}

bool LoopOGGGenerator::splitSMF() {
    std::string smfname = QFile::encodeName(smf.fileName()).toStdString();
    MIDIInfoCollector mic(smfname);
    QString outputFileNameBase = getFileNameBase();

    // split SMF by using LoopMIDIModifier::split();
    return LoopMIDIModifier::split(mic, outputFileNameBase + IntroSMFSuffix,
                                   outputFileNameBase + FirstLoopSMFSuffix,
                                   outputFileNameBase + AfterLoopSMFSuffix);
}

bool LoopOGGGenerator::convertSMFToWAV() {
    // spawn timidity++
    QString fileNameBase = getFileNameBase();

    if (this->needSplit) {
        QStringList command;
        QString inputName;
        QString outputName;
        inputName = fileNameBase + IntroSMFSuffix;
        outputName = fileNameBase + IntroWAVSuffix;
        this->timidityCommandBuilder.build(command, inputName, TiMidityDevice::RIFFWave, outputName);

        if (QProcess::execute(timidityXXBinary, command) != 0) return false;

        command.clear();
        inputName.clear();
        outputName.clear();

        inputName = fileNameBase + FirstLoopSMFSuffix;
        outputName = fileNameBase + FirstLoopWAVSuffix;
        this->timidityCommandBuilder.build(command, inputName, TiMidityDevice::RIFFWave, outputName);

        if (QProcess::execute(timidityXXBinary, command) != 0) return false;

    } else {
        QStringList command;
        QString inputFile = this->smf.fileName();
        if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
            // inputFile = QFile::encodeName(inputFile);
            inputFile.remove(0, 1);
        }
        QString outputName;
        outputName = fileNameBase + FirstLoopWAVSuffix;
        this->timidityCommandBuilder.build(command, inputFile, TiMidityDevice::RIFFWave, outputName);

        return QProcess::execute(timidityXXBinary, command) == 0;
    }

    return true;
}

bool LoopOGGGenerator::generateLoopWAV() {
    QString filenameBase = getFileNameBase();

    // create intro source wav file object
    QFile IntroSourceWAV(filenameBase + IntroWAVSuffix);

    // create first loop source wav file object
    QFile FirstLoopSourceWAV(filenameBase + FirstLoopWAVSuffix);

    // create after loop source wav file object (currently unused)
    QFile AfterLoopSourceWAV(filenameBase + AfterLoopWAVSuffix);

    // create complete loop wav file destination object
    QFile CompleteLoopWAV(filenameBase + CompleteLoopWAVSuffix);

    if (this->needSplit) {
        // TODO: implement wav generation process (Intro + Loop + Loop)
        RIFFWaveEditor rweIntro;
        RIFFWaveEditor rweLoop;

        // open source wav files
        rweIntro.open(IntroSourceWAV);
        rweLoop.open(FirstLoopSourceWAV);

        // get true loop start
        int introToLoopOS = rweIntro.getOverlap(this->loopStartOnMIDI);
        int loopToLoopOS
            = rweLoop.getOverlap(this->loopLength + this->loopOffset);
        this->loopStart
            = this->loopStartOnMIDI + qMax(introToLoopOS, loopToLoopOS);

        // mix wav with form of Intro + Loop + Loop
        rweIntro.mixAt(rweLoop, this->loopStartOnMIDI + this->loopOffset);
        rweIntro.mixAt(rweLoop, this->loopStartOnMIDI + this->loopLength
                                    + this->loopOffset);

        // save to complete loop wave
        //        rweIntro.save(CompleteLoopWAV);
        saveWAVWithTailProcess(rweIntro);
    } else {
        RIFFWaveEditor rwe;
        rwe.open(FirstLoopSourceWAV);
        this->loopStart = this->loopStartOnMIDI
                          + rwe.getOverlap(this->loopLength + this->loopOffset);
        rwe.mixAt(rwe, this->loopLength + this->loopOffset);
        //        rwe.save(CompleteLoopWAV);
        saveWAVWithTailProcess(rwe);
    }

    return true;
}

bool LoopOGGGenerator::saveWAVWithTailProcess(RIFFWaveEditor &savingWAV) {
    QSettings s;
    QString tailProcessMode = s.value("output/mode").toString();
    quint32 remainingSamplesAfterLoop
        = s.value("output/maxSamplesAfterLoopEnd").toUInt();
    qreal fadeoutStartSec = s.value("output/fadeoutStartSec").toReal();
    qreal fadeoutLengthSec = s.value("output/fadeoutLengthSec").toReal();
    quint32 loopNum = s.value("output/loopNumber").toUInt();

    QFile CompleteLoopWAV(getFileNameBase() + CompleteLoopWAVSuffix);

    if (tailProcessMode == "optimized") {  // optimized mode for game use
        // cutout unnecessary samples
        savingWAV.cutoutAfter(this->loopStart + this->loopLength
                              + remainingSamplesAfterLoop);
        // save to file
        return savingWAV.save(CompleteLoopWAV);

    } else if (tailProcessMode == "soundtrack") {  // sound-track mode
        // apply fade out process
        quint32 fadeoutStartSample = qRound(44100.0f * fadeoutStartSec);
        quint32 fadeoutLengthSample = qRound(44100.0f * fadeoutLengthSec);
        savingWAV.safeLinearFadeoutWithLoop(
            this->loopStart + this->loopLength + fadeoutStartSample,
            fadeoutLengthSample, this->loopStart, this->loopLength);

        // save to file with loopNum-times loop
        return savingWAV.saveWithLoop(CompleteLoopWAV, this->loopStart,
                                      this->loopLength, loopNum);
    }

    return false;
}

bool LoopOGGGenerator::convertWAVToOGGWithLoopTag() {
    QString outputFilename = getFileNameBase(outputDir);
    QString filenameBase = getFileNameBase();

//    QStringList command;
//    QString loopStartStr = QString::asprintf("LOOPSTART=%u", this->loopStart);
//    QString loopLengthStr
//        = QString::asprintf("LOOPLENGTH=%u", this->loopLength);
//    command << "-c" << loopStartStr;
//    command << "-c" << loopLengthStr;
//    command << "-Q";  // enable quiet mode
//    command << "-q"
//            << "4";  // TODO: able to set by configuration
//    command << "-o" << outputFilename;
//    command << filenameBase + CompleteLoopWAVSuffix;

//    // spawn oggenc
//    return QProcess::execute(oggEncBinary, command) == 0;
    return this->m_encoderExecutor->execute(filenameBase + CompleteLoopWAVSuffix, outputFilename, this->loopStart, this->loopLength) == 0;
}

bool LoopOGGGenerator::resaveWAV() {
    QString outputFilename = getFileNameBase(outputDir) + ".wav";
    QFile outputFileDest(outputFilename);
    if (outputFileDest.exists()) outputFileDest.remove();

    return QDir::root().rename(getFileNameBase() + CompleteLoopWAVSuffix,
                               outputFilename);
}

QString LoopOGGGenerator::getFileNameBase() {
    return this->getFileNameBase(this->tempFileDest.absolutePath());
}

QString LoopOGGGenerator::getFileNameBase(QString outputPath) {
    QFileInfo fi(this->smf);
    QString filename(fi.fileName());
    filename.remove(".mid");
    QString filenameBase = outputPath + QDir::separator() + filename;
    if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
        if (filenameBase.indexOf('/') == 0)
            filenameBase.remove(0, 1);  // remove slash on head
    }

    return filenameBase;
}

void LoopOGGGenerator::saveLoopInformation() {
    QString liFileName = getFileNameBase(outputDir) + LoopInformationSuffix;
    QFile loopInfoFile(liFileName);
    QJsonDocument json;
    QJsonObject obj;

    // write loop information to JSON Object
    obj.insert("LOOPSTART", static_cast<qint64>(this->loopStart));
    obj.insert("LOOPLENGTH", static_cast<qint64>(this->loopLength));
    json.setObject(obj);

    // open file in write mode
    if (!loopInfoFile.open(QIODevice::WriteOnly)) return;

    // write JSON Document to file
    loopInfoFile.write(json.toJson());
}

void LoopOGGGenerator::sweepTemporaryFiles() {
    // remove all temporary files
    QString fileNameBase = getFileNameBase();
    if (QFileInfo::exists(fileNameBase + IntroSMFSuffix))
        QFile::remove(fileNameBase + IntroSMFSuffix);
    if (QFileInfo::exists(fileNameBase + IntroWAVSuffix))
        QFile::remove(fileNameBase + IntroWAVSuffix);
    if (QFileInfo::exists(fileNameBase + FirstLoopSMFSuffix))
        QFile::remove(fileNameBase + FirstLoopSMFSuffix);
    if (QFileInfo::exists(fileNameBase + FirstLoopWAVSuffix))
        QFile::remove(fileNameBase + FirstLoopWAVSuffix);
    if (QFileInfo::exists(fileNameBase + AfterLoopSMFSuffix))
        QFile::remove(fileNameBase + AfterLoopSMFSuffix);
    if (QFileInfo::exists(fileNameBase + AfterLoopWAVSuffix))
        QFile::remove(fileNameBase + AfterLoopWAVSuffix);
    if (QFileInfo::exists(fileNameBase + CompleteLoopWAVSuffix))
        QFile::remove(fileNameBase + CompleteLoopWAVSuffix);
}