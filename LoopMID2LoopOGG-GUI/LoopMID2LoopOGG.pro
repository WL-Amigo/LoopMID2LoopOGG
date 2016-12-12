#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T22:24:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LoopMID2LoopOGG
TEMPLATE = app
CONFIG += c++11


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/ConverterSpawnGraphicsView.cpp \
    src/ConvertingDialog.cpp \
    src/ConvertingWorker.cpp

INCLUDEPATH += includes/
HEADERS  += includes/mainwindow.h \
    includes/ConverterSpawnGraphicsView.hpp \
    includes/ConvertingDialog.hpp \
    includes/ConvertingWorker.hpp

FORMS    += forms/mainwindow.ui \
    convertingdialog.ui

# Link to LoopMID2LoopOGG-CLI

SOURCES += ../LoopMID2LoopOGG-CLI/src/LoopMIDIModifier.cpp \
        ../LoopMID2LoopOGG-CLI/src/LoopOGGGenerator.cpp \
        ../LoopMID2LoopOGG-CLI/src/RIFFWaveEditor.cpp \
        ../LoopMID2LoopOGG-CLI/src/midiinfocollector.cpp \
        ../LoopMID2LoopOGG-CLI/src/midisettings.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/Binasc.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiEvent.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiEventList.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiFile.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/MidiMessage.cpp \
        ../LoopMID2LoopOGG-CLI/libs/midifile/src-library/Options.cpp

INCLUDEPATH += ../LoopMID2LoopOGG-CLI/includes/ \
        ../LoopMID2LoopOGG-CLI/libs/midifile/include/
