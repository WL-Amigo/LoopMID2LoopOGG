#-------------------------------------------------
#
# Project created by QtCreator 2015-06-22T10:17:36
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OggLoopPlayer
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += external/include

SOURCES += main.cpp\
        mainwindow.cpp \
    multibufferedoutput.cpp \
    oggloopdecoder.cpp \
    external/src_ogg/bitwise.c \
    external/src_ogg/framing.c \
    external/src_vorbis/analysis.c \
    external/src_vorbis/bitrate.c \
    external/src_vorbis/block.c \
    external/src_vorbis/codebook.c \
    external/src_vorbis/envelope.c \
    external/src_vorbis/floor0.c \
    external/src_vorbis/floor1.c \
    external/src_vorbis/info.c \
    external/src_vorbis/lookup.c \
    external/src_vorbis/lpc.c \
    external/src_vorbis/lsp.c \
    external/src_vorbis/mapping0.c \
    external/src_vorbis/mdct.c \
    external/src_vorbis/psy.c \
    external/src_vorbis/registry.c \
    external/src_vorbis/res0.c \
    external/src_vorbis/sharedbook.c \
    external/src_vorbis/smallft.c \
    external/src_vorbis/synthesis.c \
    external/src_vorbis/vorbisenc.c \
    external/src_vorbis/vorbisfile.c \
    external/src_vorbis/window.c \
    streamplayer.cpp

HEADERS  += mainwindow.hpp \
    external/include/ogg/ogg.h \
    external/include/ogg/os_types.h \
    external/include/vorbis/codec.h \
    external/include/vorbis/vorbisenc.h \
    external/include/vorbis/vorbisfile.h \
    external/src_vorbis/books/coupled/res_books_51.h \
    external/src_vorbis/books/coupled/res_books_stereo.h \
    external/src_vorbis/books/floor/floor_books.h \
    external/src_vorbis/books/uncoupled/res_books_uncoupled.h \
    external/src_vorbis/modes/floor_all.h \
    external/src_vorbis/modes/psych_11.h \
    external/src_vorbis/modes/psych_16.h \
    external/src_vorbis/modes/psych_44.h \
    external/src_vorbis/modes/psych_8.h \
    external/src_vorbis/modes/residue_16.h \
    external/src_vorbis/modes/residue_44.h \
    external/src_vorbis/modes/residue_44p51.h \
    external/src_vorbis/modes/residue_44u.h \
    external/src_vorbis/modes/residue_8.h \
    external/src_vorbis/modes/setup_11.h \
    external/src_vorbis/modes/setup_16.h \
    external/src_vorbis/modes/setup_22.h \
    external/src_vorbis/modes/setup_32.h \
    external/src_vorbis/modes/setup_44.h \
    external/src_vorbis/modes/setup_44p51.h \
    external/src_vorbis/modes/setup_44u.h \
    external/src_vorbis/modes/setup_8.h \
    external/src_vorbis/modes/setup_X.h \
    external/src_vorbis/backends.h \
    external/src_vorbis/bitrate.h \
    external/src_vorbis/codebook.h \
    external/src_vorbis/codec_internal.h \
    external/src_vorbis/envelope.h \
    external/src_vorbis/highlevel.h \
    external/src_vorbis/lookup.h \
    external/src_vorbis/lookup_data.h \
    external/src_vorbis/lpc.h \
    external/src_vorbis/lsp.h \
    external/src_vorbis/masking.h \
    external/src_vorbis/mdct.h \
    external/src_vorbis/misc.h \
    external/src_vorbis/os.h \
    external/src_vorbis/psy.h \
    external/src_vorbis/registry.h \
    external/src_vorbis/scales.h \
    external/src_vorbis/smallft.h \
    external/src_vorbis/window.h \
    multibufferedoutput.hpp \
    oggloopdecoder.h \
    streamplayer.hpp

CONFIG += mobility
MOBILITY = 

