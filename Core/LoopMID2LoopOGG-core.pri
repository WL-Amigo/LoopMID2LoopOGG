QT += core

INCLUDEPATH += $$PWD/includes/ \
    $$PWD/libs/midifile/include/

SOURCES += $$PWD/src/midiinfocollector.cpp \
    $$PWD/src/midisettings.cpp \
    $$PWD/src/RIFFWaveEditor.cpp \
    $$PWD/src/LoopOGGGenerator.cpp \
    $$PWD/src/LoopMIDIModifier.cpp \
    $$PWD/src/TiMidityCommandBuilder.cpp \
    $$PWD/src/EncoderExecutor.cpp \
    $$PWD/src/OggVorbisEncoderExecutor.cpp \
    $$PWD/libs/midifile/src-library/Binasc.cpp \
    $$PWD/libs/midifile/src-library/MidiEvent.cpp \
    $$PWD/libs/midifile/src-library/MidiEventList.cpp \
    $$PWD/libs/midifile/src-library/MidiFile.cpp \
    $$PWD/libs/midifile/src-library/MidiMessage.cpp \
    $$PWD/libs/midifile/src-library/Options.cpp \
    $$PWD/src/QAACEncoderExecutor.cpp \
    $$PWD/src/MultipleEncoderExecutor.cpp \
    $$PWD/src/Utils.cpp

HEADERS += \
    $$PWD/includes/MIDIInfoCollector.hpp \
    $$PWD/includes/midisettings.h \
    $$PWD/includes/RIFFWaveEditor.h \
    $$PWD/includes/LoopOGGGenerator.hpp \
    $$PWD/includes/LoopMIDIModifier.hpp \
    $$PWD/includes/TiMidityCommandBuilder.hpp \
    $$PWD/includes/EncoderExecutor.hpp \
    $$PWD/includes/OggVorbisEncoderExecutor.hpp \
    $$PWD/includes/QAACEncoderExecutor.hpp \
    $$PWD/includes/MultipleEncoderExecutor.hpp \
    $$PWD/includes/Utils.h
