#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T22:24:01
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LoopMID2LoopOGG
TEMPLATE = app
CONFIG += c++17


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
include(../Core/LoopMID2LoopOGG-core.pri)

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    translations/ja.ts

linux-g++ {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path = $$PREFIX/bin

    desktop.path = $$PREFIX/share/applications/
    desktop.files += loopmid2loopogg.desktop

    icon.path = $$PREFIX/share/icons/default/256x256/apps
    icon.files += resources/LoopMID2LoopOGG.png

    INSTALLS += desktop
    INSTALLS += icon
    INSTALLS += target
}
