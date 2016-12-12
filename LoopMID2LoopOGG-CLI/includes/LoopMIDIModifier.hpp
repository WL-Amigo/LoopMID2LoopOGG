﻿#ifndef LOOPMIDIMODIFIER_H
#define LOOPMIDIMODIFIER_H

#include <QString>
#include <QFile>
#include <QVector>

#include "MIDIInfoCollector.hpp"
#include "midisettings.h"
#include "MidiFile.h"

class LoopMIDIModifier
{
public:
    LoopMIDIModifier(MIDIInfoCollector& infoCollector, QString targetIntroSMF, QString targetFirstLoopSMF, QString targetAfterLoopSMF);
    bool split();
    static bool split(MIDIInfoCollector& infoCollector, QString targetIntroSMF, QString targetFirstLoopSMF, QString targetAfterLoopSMF);

private:
    bool createIntroSMF();
    bool createFirstLoopSMF();
    bool createAfterLoopSMF();
    void writeSettings(MidiFile& target, const QVector<MIDISettings>& settings, const MIDIMasterSettings& masterSettings);

private:
    QFile sourceSMF;
    MidiFile sourceMIDIFile;
    MIDIInfoCollector& infoCollector;
    int cc111Tick;
    QFile targetIntroSMF;
    QFile targetFirstLoopSMF;
    QFile targetAfterLoopSMF;
    QVector<MIDISettings> introLastSettings;
    MIDIMasterSettings introLastMasterSettings;
    QVector<MIDISettings> loopLastSettings;
    MIDIMasterSettings loopLastMasterSettings;

};

#endif // LOOPMIDIMODIFIER_H
