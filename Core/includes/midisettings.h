#ifndef MIDISETTINGS_H
#define MIDISETTINGS_H


class MIDISettings
{
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

class MIDIMasterSettings
{
public:
    // beat related settings
    int beatNum;
    int beatDenom;
    int cpb; // Clocks per Beat
    int tsnpq; // Thirty-second notes per quarter

    double tempo;

    MIDIMasterSettings();

    bool equalTo(MIDIMasterSettings& other);
};

#endif // MIDISETTINGS_H
