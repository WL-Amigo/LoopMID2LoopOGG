#include "LoopMIDIModifier.hpp"

// constants
const quint32 SetupTickRange = 240;

LoopMIDIModifier::LoopMIDIModifier(MIDIInfoCollector& infoCollector, QString targetIntroSMF, QString targetFirstLoopSMF, QString targetAfterLoopSMF):
    sourceSMF(), targetIntroSMF(targetIntroSMF),
    targetFirstLoopSMF(targetFirstLoopSMF), targetAfterLoopSMF(targetAfterLoopSMF),
    sourceMIDIFile(), cc111Tick(0), infoCollector(infoCollector),
    introLastSettings(), introLastMasterSettings(),
    loopLastSettings(), loopLastMasterSettings()
{
    // set source SMF file name
    this->sourceSMF.setFileName(QString::fromStdString(infoCollector.getFileName()));

    // open source SMF
    if(QSysInfo::windowsVersion() != QSysInfo::WV_None){
        std::string sourceSMFWin = infoCollector.getFileName().substr(1);
        if(sourceMIDIFile.read(sourceSMFWin) == 0){
            qDebug() << "LoopMIDIModifier: read source SMF failed";
        }
    } else
        sourceMIDIFile.read(sourceSMF.fileName().toLocal8Bit().data());

    // sort midi events in source SMF
    sourceMIDIFile.sortTracks();

    // make note-on & note-off pairs on each track
    for(int track = 0; track < sourceMIDIFile.getTrackCount(); track++){
        sourceMIDIFile[track].linkNotePairs();
    }

    // find CC111 (loop start point)
    this->cc111Tick = this->infoCollector.getLoopStartTick();
}

bool LoopMIDIModifier::split(MIDIInfoCollector &infoCollector, QString targetIntroSMF, QString targetFirstLoopSMF, QString targetAfterLoopSMF){
    LoopMIDIModifier lmm(infoCollector, targetIntroSMF, targetFirstLoopSMF, targetAfterLoopSMF);
    return lmm.split();
}

bool LoopMIDIModifier::split(){
    // collect settings
    introLastSettings = infoCollector.collectSettingsAt(this->cc111Tick);
    introLastMasterSettings = infoCollector.getMasterSettingsAt(this->cc111Tick);

    int songLastTick = infoCollector.findSongLastTick();
    loopLastSettings = infoCollector.collectSettingsAt(songLastTick);
    loopLastMasterSettings = infoCollector.getMasterSettingsAt(songLastTick);

    // split source SMF
    if(!createIntroSMF()) return false;
    if(!createFirstLoopSMF()) return false;
    // TODO: splitting after loop SMF

    return true;
}

bool LoopMIDIModifier::createIntroSMF(){
    MidiFile output;
    output.absoluteTicks();

    // create tracks (# is same to source)
    output.addTrack(sourceMIDIFile.getNumTracks() - 1);
    Q_ASSERT(output.getNumTracks() == sourceMIDIFile.getNumTracks());

    // set ticks per quarter
    output.setTicksPerQuarterNote(sourceMIDIFile.getTicksPerQuarterNote());

    // write events between head to occurance of CC111
    for(int track = 0; track < sourceMIDIFile.getNumTracks(); track++){
        MidiEvent* me = nullptr;
        for(int event = 0; event < sourceMIDIFile[track].size(); event++){
            me = &(sourceMIDIFile[track][event]);
            if(me->tick >= this->cc111Tick) break;
            if(me->isController() && (*me)[1] == 111) continue; // skip CC111
            if(me->isNoteOff()) continue; // skip note off event (note-off will be inserted with note-on)

            output.addEvent(track, me->tick, *me);
            if(me->isLinked()) output.addEvent(track, me->getLinkedEvent()->tick, *(me->getLinkedEvent()));
        }
    }

    // sort events before writing out
    output.sortTracks();

    // write out SMF
    output.write(targetIntroSMF.fileName().toLocal8Bit().data());

    return true;
}

bool LoopMIDIModifier::createFirstLoopSMF(){
    MidiFile output;
    output.absoluteTicks();

    // create tracks (# is same to source)
    output.addTrack(sourceMIDIFile.getNumTracks());

    // set ticks per quarter
    output.setTicksPerQuarterNote(sourceMIDIFile.getTicksPerQuarterNote());

    // write settings at last of intro part
    this->writeSettings(output, this->introLastSettings, this->introLastMasterSettings);

    // write events from CC111 tick
    for(int track = 0; track < sourceMIDIFile.getNumTracks(); track++){
        MidiEvent* me = nullptr;
        for(int event = 0; event < sourceMIDIFile[track].size(); event++){
            me = &(sourceMIDIFile[track][event]);
            if(me->tick < this->cc111Tick) continue;
            if(me->isController() && (*me)[1] == 111) continue; // skip CC111
            if(me->isNoteOff()) continue; // skip note off event (note-off will be inserted with note-on)

            output.addEvent(track, me->tick - this->cc111Tick + SetupTickRange, *me);
            if(me->isLinked()) output.addEvent(track, me->getLinkedEvent()->tick - this->cc111Tick + SetupTickRange,
                                               *(me->getLinkedEvent()));
        }
    }

    // sort events before writing out
    output.sortTracks();

    // write out SMF
    output.write(targetFirstLoopSMF.fileName().toLocal8Bit().data());

    return true;
}

void LoopMIDIModifier::writeSettings(MidiFile &target, const QVector<MIDISettings> &settings, const MIDIMasterSettings &masterSettings){
    MidiEvent me;

    // write master settings
    //   write tempo information
    me.setMetaTempo(masterSettings.tempo);
    target.addEvent(0, 0, me);

    //   TODO: write beat information?

    // write settings for channels
    for(int channel = 0; channel < settings.size(); channel++){
        // write program change
        me.setCommand(0xC0 | (channel & 0x0F), settings[channel].program);
        Q_ASSERT(me.isPatchChange());
        target.addEvent(0, 1, me);

        // write pitch bend sensitivity
        //   write CC101 to 0
        me.setCommand(0xB0 | (channel & 0x0F), 101, 0);
        Q_ASSERT(me.isController());
        target.addEvent(0, 2, me);
        //   write CC100 to 0
        me.setCommand(0xB0 | (channel & 0x0F), 100, 0);
        Q_ASSERT(me.isController());
        target.addEvent(0, 3, me);
        //   write CC6 to pitch bend sensitivity
        me.setCommand(0xB0 | (channel & 0x0F), 6, settings[channel].pitchbendSensitivity);
        Q_ASSERT(me.isController());
        target.addEvent(0, 4, me);
        //   write CC36 to 0 (unused LSB)
        me.setCommand(0xB0 | (channel & 0x0F), 36, 0);
        Q_ASSERT(me.isController());
        target.addEvent(0, 5, me);

        // write pitch bend
        me.setCommand(0xE0 | (channel & 0x0F), settings[channel].pitchbend & 0x00FF,
                      (settings[channel].pitchbend & 0xFF00) >> 8);
        Q_ASSERT(me.isPitchbend());
        target.addEvent(0, 6, me);

        // write all controller
        for(int ctrl = 0; ctrl < 128; ctrl++){
            // skip if the controller isn't set
            if(settings[channel].controlls[ctrl] == -1) continue;
            // skip if the controller related to RPN/NRPN
            if(ctrl == 6 || ctrl == 36 || (ctrl >= 98 && ctrl <= 101)) continue;
            me.setCommand(0xB0 | (channel & 0x0F), ctrl, settings[channel].controlls[ctrl]);
            Q_ASSERT(me.isController());
            target.addEvent(0, 7 + ctrl, me);
        }
    }
}
