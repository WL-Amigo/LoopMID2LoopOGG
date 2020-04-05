#include "midisettings.h"

// ==========----------
// = MIDISettings class implementation
// ==========----------

MIDISettings::MIDISettings() { this->reset(); }

MIDISettings::MIDISettings(int channel) {
    this->reset();
    this->channel = channel;
}

void MIDISettings::reset() {
    this->channel = 0;
    this->pitchbend = 0x4000;
    this->pitchbendSensitivity = 2;
    this->program = 0;
    for (int idx = 0; idx < 128; idx++) {
        this->controlls[idx] = -1;
    }

    // set initial value for several CC
    this->controlls[0] = 0;
    this->controlls[32] = 0;
}

bool MIDISettings::equalTo(MIDISettings &other) {
    if (this->channel == other.channel && this->pitchbend == other.pitchbend &&
        this->pitchbendSensitivity == other.pitchbendSensitivity &&
        this->program == other.program) {
        for (int idx = 0; idx < 128; idx++) {
            // exclude RPN/NRPN related controlls
            if (idx == 6 || idx == 36 || idx == 98 || idx == 99 || idx == 100 ||
                idx == 101)
                continue;

            if (this->controlls[idx] != other.controlls[idx]) return false;
        }
        // all tests passed
        return true;
    }

    return false;
}

// ==========----------
// = MIDIMasterSettings class implementation
// ==========----------

MIDIMasterSettings::MIDIMasterSettings() {
    this->beatNum = 4;
    this->beatDenom = 2;
    this->cpb = 24;
    this->tsnpq = 8;
    this->tempo = 120.0;
    this->masterVolume = 100;
    this->gmSystemOn = false;
    this->gsReset = false;
}

bool MIDIMasterSettings::equalTo(MIDIMasterSettings &other) {
    return this->beatNum == other.beatNum &&
           this->beatDenom == other.beatDenom && this->cpb == other.cpb &&
           this->tsnpq == other.tsnpq && this->tempo == other.tempo &&
           this->masterVolume == other.masterVolume &&
           this->gmSystemOn == other.gmSystemOn &&
           this->gsReset == other.gmSystemOn;
}
