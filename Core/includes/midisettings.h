#ifndef MIDISETTINGS_H
#define MIDISETTINGS_H

#include <QtCore>

class MIDISettings {
public:
    int channel;
    int pitchbend;
    int controlls[128];
    int pitchbendSensitivity;
    int program;

    MIDISettings();
    MIDISettings(int channel);

    bool equalTo(MIDISettings& other);

private:
    void reset();
};

class MIDIMasterSettings {
public:
    // beat related settings
    int beatNum;
    int beatDenom;
    int cpb;    // Clocks per Beat
    int tsnpq;  // Thirty-second notes per quarter

    double tempo;
    quint8 masterVolume;
    bool gmSystemOn;
    bool gsReset;

    MIDIMasterSettings();

    bool equalTo(MIDIMasterSettings& other);
};

#endif  // MIDISETTINGS_H
