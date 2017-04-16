#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T22:24:01
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LoopMID2LoopOGG
TEMPLATE = app
CONFIG += c++11


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/ConverterSpawnGraphicsView.cpp \
    src/ConvertingDialog.cpp \
    src/ConvertingWorker.cpp \
    src/conversionconfirmationdialog.cpp \
    src/ConfigDialog.cpp \
    src/MultiConvertingDialog.cpp \
    src/TiMidityCommandBuilderProvider.cpp \
    src/EncoderExecutorProvider.cpp

INCLUDEPATH += includes/
HEADERS  += includes/mainwindow.h \
    includes/ConverterSpawnGraphicsView.hpp \
    includes/ConvertingDialog.hpp \
    includes/ConvertingWorker.hpp \
    includes/conversionconfirmationdialog.h \
    includes/ConfigDialog.hpp \
    includes/MultiConvertingDialog.hpp \
    includes/GlobalConstants.hpp \
    includes/TiMidityCommandBuilderProvider.h \
    includes/EncoderExecutorProvider.hpp


FORMS    += forms/mainwindow.ui \
    forms/convertingdialog.ui \
    forms/conversionconfirmationdialog.ui \
    forms/ConfigDialog.ui \
    forms/MultiConvertingDialog.ui

# Link to LoopMID2LoopOGG-CLI

SOURCES += ../LoopMID2LoopOGG-CLI/src/LoopMIDIModifier.cpp \
        ../LoopMID2LoopOGG-CLI/src/LoopOGGGenerator.cpp \
        ../LoopMID2LoopOGG-CLI/src/RIFFWaveEditor.cpp \
        ../LoopMID2LoopOGG-CLI/src/midiinfocollector.cpp \
        ../LoopMID2LoopOGG-CLI/src/midisettings.cpp \
        ../LoopMID2LoopOGG-CLI/src/TiMidityCommandBuilder.cpp \
        ../LoopMID2LoopOGG-CLI/src/EncoderExecutor.cpp \
        ../LoopMID2LoopOGG-CLI/src/OggVorbisEncoderExecutor.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/Binasc.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiEvent.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiEventList.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiFile.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiMessage.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/Options.cpp

HEADERS += ../LoopMID2LoopOGG-CLI/includes/LoopMIDIModifier.hpp \
        ../LoopMID2LoopOGG-CLI/includes/LoopOGGGenerator.hpp \
        ../LoopMID2LoopOGG-CLI/includes/MIDIInfoCollector.hpp \
        ../LoopMID2LoopOGG-CLI/includes/midisettings.h \
        ../LoopMID2LoopOGG-CLI/includes/RIFFWaveEditor.h

INCLUDEPATH += ../LoopMID2LoopOGG-CLI/includes/ \
        ../LoopMID2LoopOGG-CLI/libs/midifile/include/

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    translations/ja.ts
