#ifndef LOOPMIDIMODIFIER_H
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
    void copyEventProperly(MidiFile &target, MidiEvent* sourceEvent, int track, int offsetTick);
    void writeSettings(MidiFile& target, const QVector<MIDISettings>& settings, const MIDIMasterSettings& masterSettings);
    /**
     * @returns next event tick
     */
    int writeProgramChange(MidiFile& target, int channel, int eventTick, quint8 program, quint8 bankMSB, quint8 bankLSB);
    /**
     * @returns next event tick
     */
    int writePitchBendSensitivity(MidiFile& target, int channel, int eventTick, quint8 pitchbendSensitivity);
    /**
     * @returns next event tick
     */
    int writePitchBend(MidiFile& target, int channel, int eventTick, int pitchbend);
    /**
     * @returns next event tick
     */
    int writeControlChange(MidiFile& target, int channel, int eventTick, quint8 controlNum, int controlValue);

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
