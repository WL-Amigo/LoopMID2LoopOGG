﻿#include "LoopMIDIModifier.hpp"
#include <QDebug>

// constants
const quint32 SetupTickRange = 480;

LoopMIDIModifier::LoopMIDIModifier(MIDIInfoCollector &infoCollector,
                                   QString targetIntroSMF,
                                   QString targetFirstLoopSMF,
                                   QString targetAfterLoopSMF)
    : sourceSMF(),
      targetIntroSMF(targetIntroSMF),
      targetFirstLoopSMF(targetFirstLoopSMF),
      targetAfterLoopSMF(targetAfterLoopSMF),
      sourceMIDIFile(),
      cc111Tick(0),
      infoCollector(infoCollector),
      introLastSettings(),
      introLastMasterSettings(),
      loopLastSettings(),
      loopLastMasterSettings() {
    // set source SMF file name
    this->sourceSMF.setFileName(
        QString::fromStdString(infoCollector.getFileName()));

    // open source SMF
    if (QSysInfo::windowsVersion() != QSysInfo::WV_None) {
        std::string sourceSMFWin = infoCollector.getFileName().substr(1);
        if (sourceMIDIFile.read(sourceSMFWin) == 0) {
            qDebug() << "LoopMIDIModifier: read source SMF failed";
        }
    } else
        sourceMIDIFile.read(sourceSMF.fileName().toLocal8Bit().data());

    // sort midi events in source SMF
    sourceMIDIFile.sortTracks();

    // make note-on & note-off pairs on each track
    for (int track = 0; track < sourceMIDIFile.getTrackCount(); track++) {
        sourceMIDIFile[track].linkNotePairs();
    }

    // find CC111 (loop start point)
    this->cc111Tick = this->infoCollector.getLoopStartTick();
}

bool LoopMIDIModifier::split(MIDIInfoCollector &infoCollector,
                             QString targetIntroSMF, QString targetFirstLoopSMF,
                             QString targetAfterLoopSMF) {
    LoopMIDIModifier lmm(infoCollector, targetIntroSMF, targetFirstLoopSMF,
                         targetAfterLoopSMF);
    return lmm.split();
}

bool LoopMIDIModifier::split() {
    // collect settings
    introLastSettings = infoCollector.collectSettingsAt(this->cc111Tick);
    introLastMasterSettings =
        infoCollector.getMasterSettingsAt(this->cc111Tick);

    int songLastTick = infoCollector.findSongLastTick();
    loopLastSettings = infoCollector.collectSettingsAt(songLastTick);
    loopLastMasterSettings = infoCollector.getMasterSettingsAt(songLastTick);

    // split source SMF
    if (!createIntroSMF()) return false;
    if (!createFirstLoopSMF()) return false;
    // TODO: splitting after loop SMF

    return true;
}

bool LoopMIDIModifier::createIntroSMF() {
    MidiFile output;
    output.absoluteTicks();

    // create tracks (# is same to source)
    output.addTrack(sourceMIDIFile.getNumTracks() - 1);
    Q_ASSERT(output.getNumTracks() == sourceMIDIFile.getNumTracks());

    // set ticks per quarter
    output.setTicksPerQuarterNote(sourceMIDIFile.getTicksPerQuarterNote());

    // write events between head to occurance of CC111
    for (int track = 0; track < sourceMIDIFile.getNumTracks(); track++) {
        MidiEvent *me = nullptr;
        for (int event = 0; event < sourceMIDIFile[track].size(); event++) {
            me = &(sourceMIDIFile[track][event]);
            if (me->tick >= this->cc111Tick) break;
            this->copyEventProperly(output, me, track, 0);
        }
    }

    // sort events before writing out
    output.sortTracks();

    // write out SMF
    output.write(targetIntroSMF.fileName().toLocal8Bit().data());

    return true;
}

bool LoopMIDIModifier::createFirstLoopSMF() {
    MidiFile output;
    output.absoluteTicks();

    // create tracks (# is same to source)
    output.addTrack(sourceMIDIFile.getNumTracks());

    // set ticks per quarter
    output.setTicksPerQuarterNote(sourceMIDIFile.getTicksPerQuarterNote());

    // write settings at last of intro part
    this->writeSettings(output, this->introLastSettings,
                        this->introLastMasterSettings);

    // write events from CC111 tick
    for (int track = 0; track < sourceMIDIFile.getNumTracks(); track++) {
        MidiEvent *me = nullptr;
        for (int event = 0; event < sourceMIDIFile[track].size(); event++) {
            me = &(sourceMIDIFile[track][event]);
            if (me->tick < this->cc111Tick) continue;
            this->copyEventProperly(output, me, track,
                                    SetupTickRange - this->cc111Tick);
        }
    }

    // sort events before writing out
    output.sortTracks();

    // write out SMF
    output.write(targetFirstLoopSMF.fileName().toLocal8Bit().data());

    return true;
}

void LoopMIDIModifier::copyEventProperly(MidiFile &target,
                                         MidiEvent *sourceEvent, int track,
                                         int offsetTick) {
    if (sourceEvent->isController() && (*sourceEvent)[1] == 111)
        return;  // skip CC111
    if (sourceEvent->isNoteOff())
        return;  // skip note off event (note-off will be inserted
                 // with note-on)
    if (sourceEvent->isController() &&
        ((*sourceEvent)[1] == 0 || (*sourceEvent)[1] == 32)) {
        // If event is Control Change related to Bank Select,
        // set sequence marker to -1 or 0 to avoid performing Program Change before Bank Select
        // (Sequence marker on events are already assigned sequencially)
        sourceEvent->seq = (*sourceEvent)[1] == 0 ? -1 : 0;
    }

    int newEventIndex = target.addEvent(track, sourceEvent->tick + offsetTick, *sourceEvent);
    // write seq to new event
    target.getEvent(track, newEventIndex).seq = sourceEvent->seq;
    if (sourceEvent->isNoteOn()) {
        if (sourceEvent->isLinked()) {
            // add correspond note-off
            target.addEvent(track,
                            sourceEvent->getLinkedEvent()->tick + offsetTick,
                            *(sourceEvent->getLinkedEvent()));
        } else {
            qDebug().noquote() << "LoopMIDIModifier: find note-on event which "
                                  "don't have correspond note-off";
            // interpolate correspond note-off (assume tick as End of Track)
            MidiEvent noteoffOnEOT(*sourceEvent);
            noteoffOnEOT.setCommandNibble(0x8);
            target.addEvent(track,
                            this->infoCollector.findSongLastTick() + offsetTick,
                            noteoffOnEOT);
        }
    }
}

void LoopMIDIModifier::writeSettings(MidiFile &target,
                                     const QVector<MIDISettings> &settings,
                                     const MIDIMasterSettings &masterSettings) {
    MidiEvent me;
    MidiEvent sysex;

    // write master settings
    //   write tempo information
    me.setMetaTempo(masterSettings.tempo);
    target.addEvent(0, 0, me);
    //   write GM System On
    if (masterSettings.gmSystemOn) {
        sysex.clear();
        sysex.insert(sysex.end(), {0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7});
        target.addEvent(0, 0, sysex);
    }
    // write GS Reset
    if (masterSettings.gsReset) {
        sysex.clear();
        sysex.insert(sysex.end(), {0xF0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x00,
                                   0x7F, 0x00, 0x41, 0xF7});
        target.addEvent(0, 60, sysex);
    }
    //   write master volume setting
    if (masterSettings.masterVolume != 100) {
        sysex.clear();
        sysex.insert(sysex.end(),
                     {0xF0, 0x7F, 0x7F, 0x04, 0x01, 0x00, 0x00, 0xF7});
        sysex[6] = masterSettings.masterVolume & static_cast<quint8>(0x7F);
        target.addEvent(0, 120, sysex);
    }

    //   TODO: write beat information?
    const int sysExPadding = 121;

    // write settings for channels
    for (int channel = 0; channel < settings.size(); channel++) {
        int eventOffset = sysExPadding;
        auto& channelSettings = settings[channel];

        // write program change
        eventOffset = this->writeProgramChange(target, channel, eventOffset, channelSettings.program, channelSettings.controlls[0], channelSettings.controlls[32]);

        // write pitch bend sensitivity
        eventOffset = this->writePitchBendSensitivity(target, channel, eventOffset, channelSettings.pitchbendSensitivity);

        // write pitch bend
        eventOffset = this->writePitchBend(target, channel, eventOffset, channelSettings.pitchbend);

        // write all controller
        // (discard after 120 because they have special effect)
        for (int ctrl = 0; ctrl < 120; ctrl++) {
            // skip if the controller isn't set
            if (settings[channel].controlls[ctrl] == -1) continue;
            // skip if the controller related to ...
            // * RPN/NRPN
            // * Bank Select MSB/LSB
            // * Data Entry MSB/LSB
            if (ctrl == 0 || ctrl == 32 || ctrl == 6 || ctrl == 36 ||
                (ctrl >= 98 && ctrl <= 101))
              continue;

            eventOffset = this->writeControlChange(target, channel, eventOffset, ctrl, channelSettings.controlls[ctrl]);
        }
    }
}

int LoopMIDIModifier::writeProgramChange(MidiFile& target, int channel, int eventTick, quint8 program, quint8 bankMSB, quint8 bankLSB){
    MidiEvent me;

    // write bank select MSB/LSB
    eventTick = this->writeControlChange(target, channel, eventTick, 0, bankMSB);
    eventTick = this->writeControlChange(target, channel, eventTick, 32, bankLSB);

    // write program change
    me.setCommand(0xC0 | (channel & 0x0F), program);
    Q_ASSERT(me.isPatchChange());
    target.addEvent(0, eventTick++, me);

    return eventTick;
}

int LoopMIDIModifier::writePitchBendSensitivity(MidiFile& target, int channel, int eventTick, quint8 pitchbendSensitivity){
    MidiEvent me;

    // write pitch bend sensitivity
    //   write CC101 to 0
    me.setCommand(0xB0 | (channel & 0x0F), 101, 0);
    Q_ASSERT(me.isController());
    target.addEvent(0, eventTick++, me);
    //   write CC100 to 0
    me.setCommand(0xB0 | (channel & 0x0F), 100, 0);
    Q_ASSERT(me.isController());
    target.addEvent(0, eventTick++, me);
    //   write CC6 to pitch bend sensitivity
    me.setCommand(0xB0 | (channel & 0x0F), 6,
                  pitchbendSensitivity);
    Q_ASSERT(me.isController());
    target.addEvent(0, eventTick++, me);
    //   write CC36 to 0 (unused LSB)
    me.setCommand(0xB0 | (channel & 0x0F), 36, 0);
    Q_ASSERT(me.isController());
    target.addEvent(0, eventTick++, me);

    return eventTick;
}

int LoopMIDIModifier::writePitchBend(MidiFile& target, int channel, int eventTick, int pitchbend){
    MidiEvent me;

    me.setCommand(0xE0 | (channel & 0x0F),
                  pitchbend & 0x00FF,
                  (pitchbend & 0xFF00) >> 8);
    Q_ASSERT(me.isPitchbend());
    target.addEvent(0, eventTick, me);

    return eventTick + 1;
}

int LoopMIDIModifier::writeControlChange(MidiFile& target, int channel, int eventTick, quint8 controlNum, int controlValue){
    MidiEvent me;

    me.setCommand(0xB0 | (channel & 0x0F),
                  controlNum,
                  controlValue);
    Q_ASSERT(me.isController());
    target.addEvent(0, eventTick, me);

    return eventTick + 1;
}
