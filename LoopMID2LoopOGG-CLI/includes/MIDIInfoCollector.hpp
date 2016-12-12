#ifndef MIDIINFOCOLLECTOR_H
#define MIDIINFOCOLLECTOR_H

#include <QDebug>
#include <QVector>
#include <string>
#include "MidiFile.h"
#include "midisettings.h"
#include <QFile>

class MIDIInfoCollector
{
public:
    MIDIInfoCollector(std::string& filename);
    std::string& getFileName();
    void test();
    bool analyse();
    int getLoopStartSample(int sampleRate);
    int getLoopLengthSample(int sampleRate);
    int getLoopAppendOffsetSample(int sampleRate);
    bool hasIntro();
    MidiEvent* getLoopStartControl();
    int getLoopStartTick();
    QVector<MIDISettings> collectSettingsAt(int absTick);
    MIDIMasterSettings getMasterSettingsAt(int absTick);
    int findSongLastTick();
    int findSongStartTick();
    int findFirstNoteOnTickAfter(int tick);

private: // methods
    void getAllEOT();
    void getAllEventOn(quint32 tick);

private: // variables
    std::string filename;
    MidiFile midifile;

};

#endif // MIDIINFOCOLLECTOR_H
