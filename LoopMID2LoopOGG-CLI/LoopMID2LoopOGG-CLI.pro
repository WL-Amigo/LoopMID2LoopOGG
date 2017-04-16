QT += core
QT -= gui

CONFIG += c++11

TARGET = LoopMID2LoopOGG-CLI
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += includes/ \
    libs/midifile/include/

SOURCES += src/main.cpp \
    src/midiinfocollector.cpp \
    src/midisettings.cpp \
    src/RIFFWaveEditor.cpp \
    src/LoopOGGGenerator.cpp \
    src/LoopMIDIModifier.cpp \
    src/TiMidityCommandBuilder.cpp \
    src/EncoderExecutor.cpp \
    src/OggVorbisEncoderExecutor.cpp


HEADERS += \
    includes/MIDIInfoCollector.hpp \
    includes/midisettings.h \
    includes/RIFFWaveEditor.h \
    includes/LoopOGGGenerator.hpp \
    includes/LoopMIDIModifier.hpp \
    includes/TiMidityCommandBuilder.hpp \
    includes/EncoderExecutor.hpp \
    includes/OggVorbisEncoderExecutor.hpp

DEPENDPATH += libs/midifile/include/

SOURCES += libs/midifile/src-library/Binasc.cpp \
    libs/midifile/src-library/MidiEvent.cpp \
    libs/midifile/src-library/MidiEventList.cpp \
    libs/midifile/src-library/MidiFile.cpp \
    libs/midifile/src-library/MidiMessage.cpp \
    libs/midifile/src-library/Options.cpp
