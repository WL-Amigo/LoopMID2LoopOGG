#include <math.h>
#include <QString>
#include "MIDIInfoCollector.hpp"
#include <QSysInfo>

MIDIInfoCollector::MIDIInfoCollector(std::string &filename) {
    this->filename = filename;

    // attempt to read given file
    if(QSysInfo::windowsVersion() != QSysInfo::WV_None){
        std::string filenameWin = filename.substr(1);
        this->midifile.read(filenameWin);
    } else
        this->midifile.read(filename);

    // convert delta to absolute if tick count mode is delta
    if (!(this->midifile.isAbsoluteTicks())) {
        this->midifile.absoluteTicks();
    }

    // sort all tracks for proper analysis
    this->midifile.sortTracks();

    // time analysis by MIDIFile library
    this->midifile.doTimeAnalysis();

    qDebug() << "Ticks Per Quarter: "
             << this->midifile.getTicksPerQuarterNote();
    qDebug() << "Tracks: " << this->midifile.getTrackCount();
}

std::string& MIDIInfoCollector::getFileName() { return this->filename; }

MidiEvent *MIDIInfoCollector::getLoopStartControl() {
    // merge all tracks for analysis
    this->midifile.joinTracks();

    // search CC111
    MidiEvent *event = nullptr;
    MidiEvent *cc111 = nullptr;
    for (int idx = 0; idx < midifile[0].size(); idx++) {
        event = &midifile[0][idx];
        if (event->isController()) {
            if ((*event)[1] == 111) {
                cc111 = event;
                break;
            }
        }
    }

    // split joined track
    this->midifile.splitTracks();

    return cc111;
}

int MIDIInfoCollector::getLoopStartTick() {
    auto cc111 = getLoopStartControl();
    if(cc111 != nullptr) return cc111->tick;

    // if CC111 was not found, assumes that loop start tick is equal to song start tick
    return this->findSongStartTick();
}

int MIDIInfoCollector::findSongLastTick() {
    // merge all tracks for analysis
    this->midifile.joinTracks();

    // re-sort joined track
    this->midifile.sortTrack(midifile[0]);

    // find last event and get tick
    MidiEvent *event = nullptr;
    // case: last of song is last note-off
    /*
    for(int idx = this->midifile[0].size() - 1; idx > 0; idx--){
        event = &this->midifile[0][idx];
        if(event->isNoteOff()) break;
    }
    */
    // case: last of song is EOT
    for(int idx = this->midifile[0].size() - 1; idx > 0; idx--){
        event = &this->midifile[0][idx];
        if(event->isMeta() && (*event)[1] == 0x2F && (*event)[2] == 0x00) break;
    }
    int songLastTick = event->tick;

    // split joined track
    this->midifile.splitTracks();

    qDebug() << "song last tick:" << songLastTick;
    qDebug() << "event track:" << event->track;
    qDebug() << "event dump:" << QString::asprintf("%02X %02X %02X", (*event)[0], (*event)[1], (*event)[2]);

    return songLastTick;
}

int MIDIInfoCollector::findFirstNoteOnTickAfter(int tick) {
    // merge all tracks for analysis
    this->midifile.joinTracks();

    // re-sort joined track
    this->midifile.sortTrack(midifile[0]);

    // find first note-on event and get tick
    MidiEvent *event = nullptr;
    int retTick = 0;
    for (int idx = 0; idx < this->midifile[0].size(); idx++) {
        event = &this->midifile[0][idx];
        if (event->isNoteOn() && event->tick >= tick) {
            retTick = event->tick;
            break;
        }
    }

    // split joined track
    this->midifile.splitTracks();

    qDebug() << "Tick:" << retTick;

    return retTick;
}

int MIDIInfoCollector::findSongStartTick() {
    return this->findFirstNoteOnTickAfter(0);
}

QVector<MIDISettings> MIDIInfoCollector::collectSettingsAt(int absTick) {
    QVector<MIDISettings> settings(16);  // MIDI Channel maximum
    // set channel to each setting structure
    for (int idx = 0; idx < 16; idx++) {
        settings[idx].channel = idx;
    }

    // join tracks for analysis
    midifile.joinTracks();

    // collect midi settings information

    // trace midi events to absTick
    MidiEvent *mev = nullptr;
    for (int evIdx = 0; evIdx < this->midifile[0].size(); evIdx++) {
        mev = &midifile[0][evIdx];
        if (mev->tick > absTick) break;

        // get pitchbend
        if (mev->isPitchbend()) {
            settings[mev->getChannel()].pitchbend =
                (int)(*mev)[1] + ((int)(*mev)[2] << 8 & 0xFF00);
        }

        // get control change
        if (mev->isController())
            settings[mev->getChannel()].controlls[(*mev)[1]] = (int)(*mev)[2];

        // get pitchbend range
        if (mev->isController() && (*mev)[1] == 6 &&
            settings[mev->getChannel()].controlls[100] == 0 &&
            settings[mev->getChannel()].controlls[101] == 0) {
            settings[mev->getChannel()].pitchbendSensitivity = (int)(*mev)[2];
        }

        // get program change
        if (mev->isPatchChange())
            settings[mev->getChannel()].program = (int)(*mev)[1];
    }

    // strip joined tracks
    midifile.splitTracks();

    return settings;
}

MIDIMasterSettings MIDIInfoCollector::getMasterSettingsAt(int absTick) {
    // merge all tracks for analysis
    this->midifile.joinTracks();

    MidiEvent *event = nullptr;
    MIDIMasterSettings ret = MIDIMasterSettings();
    for (int idx = 0; idx < midifile[0].size(); idx++) {
        event = &midifile[0][idx];
        // break on given range ended
        if (event->tick > absTick) break;

        if (event->isMeta()) {
            // get beat change
            if ((*event)[1] == 0x58 && (*event)[2] == 0x04) {
                ret.beatNum = (*event)[3];
                ret.beatDenom = (*event)[4];
                ret.cpb = (*event)[5];
                ret.tsnpq = (*event)[6];
            }
            // get tempo change
            if (event->isTempo()) {
                ret.tempo = event->getTempoBPM();
                qDebug() << "Tempo Change Track:" << event->track
                         << ", to:" << ret.tempo;
            }
        }
    }

    // split joined track
    this->midifile.splitTracks();

    return ret;
}

int MIDIInfoCollector::getLoopStartSample(int sampleRate) {
    int songStartTick = this->findSongStartTick();
    int cc111Tick = this->getLoopStartTick();

    double songStartSec = this->midifile.getTimeInSeconds(songStartTick);
    double cc111Sec = this->midifile.getTimeInSeconds(cc111Tick);

    return (int)(floor((double)sampleRate * (cc111Sec - songStartSec)));
}

int MIDIInfoCollector::getLoopLengthSample(int sampleRate) {
    int songEndTick = this->findSongLastTick();
    int cc111Tick = this->getLoopStartTick();

    double songLastSec = this->midifile.getTimeInSeconds(songEndTick);
    double cc111Sec = this->midifile.getTimeInSeconds(cc111Tick);

    return (int)(floor((double)sampleRate * (songLastSec - cc111Sec)));
}

int MIDIInfoCollector::getLoopAppendOffsetSample(int sampleRate) {
    int cc111Tick = this->getLoopStartTick();

    // find first note-on event tick after cc111Tick
    int firstNoteTickAfterLoop = this->findFirstNoteOnTickAfter(cc111Tick);

    if (cc111Tick == firstNoteTickAfterLoop)
        return 0;
    else {
        double cc111Sec = this->midifile.getTimeInSeconds(cc111Tick);
        double firstNoteSecAfterLoop =
            this->midifile.getTimeInSeconds(firstNoteTickAfterLoop);
        return (int)(floor((double)sampleRate *
                           (firstNoteSecAfterLoop - cc111Sec)));
    }

    return -1;  // should not be reached
}

bool MIDIInfoCollector::hasIntro() {
    int cc111Tick = this->getLoopStartTick();
    int songStartTick = this->findSongStartTick();

    return !(cc111Tick <= songStartTick);
}

void MIDIInfoCollector::getAllEOT(){
    // merge all tracks for analysis
    this->midifile.joinTracks();

    MidiEvent *event = nullptr;
    for (int idx = 0; idx < midifile[0].size(); idx++) {
        event = &midifile[0][idx];
        if(event->isMeta() && (*event)[1] == 0x2F && (*event)[2] == 0x00){
            qDebug() << "EOT: track =" << event->track << ", tick =" << event->tick;
        }
    }

    // split joined track
    this->midifile.splitTracks();

    return;
}

void MIDIInfoCollector::getAllEventOn(quint32 tick){
    // merge all tracks for analysis
    this->midifile.joinTracks();

    MidiEvent *event = nullptr;
    for (int idx = 0; idx < midifile[0].size(); idx++) {
        event = &midifile[0][idx];
        if(event->tick == tick){
            qDebug() << "Event on " << tick << ": track =" << event->track << ", " << QString::asprintf("%02X %02X %02X", (*event)[0], (*event)[1], (*event)[2]);
        }
    }

    // split joined track
    this->midifile.splitTracks();

    return;
}

// testing function
void MIDIInfoCollector::test() {
    // retrieving CC111
    MidiEvent *cc111 = this->getLoopStartControl();
    if (cc111 != nullptr) {
        qDebug() << "CC111 is found!";
        qDebug() << "\tTrack #: " << cc111->track;
        qDebug() << "\tAbsolute Tick: " << cc111->tick;
        qDebug() << "\tController #: " << (*cc111)[1];
        qDebug() << "\tController value: " << (*cc111)[2];
    } else {
        qDebug() << "CC111 is not found...";
    }

    // retrieving song start tick
    int songStartTick = this->findSongStartTick();
    qDebug() << "Song starts on: " << songStartTick;

    // retrieving song end tick
    int songLastTick = this->findSongLastTick();
    qDebug() << "Song ends on: " << songLastTick;

    // check that the song has intro or not
    if (this->hasIntro())
        qDebug() << "Song has intro";
    else
        qDebug() << "Song doesn't have intro (loop only)";

    // retrieving track settings on song start
    /*
    QVector<MIDISettings> trackSettingsOnStart =
        this->collectSettingsAt(songStartTick);
    for (MIDISettings trackSetting : trackSettingsOnStart) {
        qDebug() << "channel #: " << trackSetting.channel;
        qDebug() << "\tprogram #: " << trackSetting.program;
        qDebug() << "\tpitchbend: " << trackSetting.pitchbend;
        qDebug() << "\tpitchbend range: " << trackSetting.pitchbendSensitivity;
        QString controllsStr = "";
        for (int idx = 0; idx < 128; idx++) {
            controllsStr +=
                QString::asprintf("%02X ", trackSetting.controlls[idx]);
        }
        qDebug() << "\tcontrolls list: " << controllsStr;
    }
    */

    // analyse beat on song start
    MIDIMasterSettings masterSettingsOnStart =
        this->getMasterSettingsAt(songStartTick);
    qDebug() << "Beat at" << songStartTick;
    qDebug() << "\tBeat numerator:" << masterSettingsOnStart.beatNum;
    qDebug() << "\tBeat denominator:" << masterSettingsOnStart.beatDenom;
    qDebug() << "\tClocks per beat:" << masterSettingsOnStart.cpb;
    qDebug() << "\t32 / 4:" << masterSettingsOnStart.tsnpq;

    // analyse loop start/length time in samples
    qDebug() << "assumed LOOPSTART (overshoot is not concerned):"
             << this->getLoopStartSample(44100);
    qDebug() << "LOOPLENGTH:" << this->getLoopLengthSample(44100);
    qDebug() << "offset of point loop WAV will be appended:"
             << this->getLoopAppendOffsetSample(44100);

    // analyse end of track
//    this->getAllEOT();

    this->getAllEventOn(84720);
}

// do analysis and store result in self fields
bool MIDIInfoCollector::analyse() { return false; }
