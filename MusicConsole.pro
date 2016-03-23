#-------------------------------------------------
#
# ** Music Console **
# ** Copyright © by GhettoGirl, 2013-2016
#
#  ~ github.com/GhettoGirl
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = MusicConsole
CONFIG   += console c++14
CONFIG   -= app_bundle

TEMPLATE = app

# link libgcc and libstdc++ statically
QMAKE_CXXFLAGS += -static-libstdc++ -static-libgcc
QMAKE_LFLAGS += -static-libstdc++ -static-libgcc

# Libraries
LIBS += -ltag \
        -lreadline

# Sources
SOURCES += main.cpp \
    Utils/contrib/SingleApplication/singleapplication.cpp \
    Utils/filesystemmodel.cpp \
    applicationmanager.cpp \
    musicconsole.cpp \
    Utils/medialibrarymodel.cpp \
    Utils/mediatagsreader.cpp \
    Utils/searchpathgen.cpp \
    Utils/pathexpander.cpp \
    SearchPathGens/unicodewhitespacefixer.cpp \
    SearchPathGens/universaljapanesekanalookup.cpp \
    Utils/searchkeys.cpp \
    configmanager.cpp \
    Sys/command.cpp \
    Commands/cmdaudio.cpp \
    Commands/cmdvideo.cpp \
    Commands/cmdmodule.cpp \
    Commands/cmdplaylist.cpp \
    Commands/cmdsearch.cpp \
    Commands/cmdbrowse.cpp \
    Commands/cmdrandom.cpp \
    Commands/cmdshuffle.cpp \
    Commands/cmdrepeat.cpp \
    Commands/cmdhistory.cpp \
    Commands/cmdstatistics.cpp \
    Commands/cmdrescan.cpp \
    Sys/mediaplayercontroller.cpp \
    Sys/kbhit.cpp \
    Sys/playlistparser.cpp \
    Sys/historymanager.cpp \
    SearchPathGens/unicodelatingen.cpp \
    Sys/mediacache.cpp

# Headers
HEADERS += \
    Utils/contrib/SingleApplication/singleapplication.h \
    Utils/filesystemmodel.hpp \
    applicationmanager.hpp \
    musicconsole.hpp \
    Utils/medialibrarymodel.hpp \
    Utils/mediatagsreader.hpp \
    Utils/searchpathgen.hpp \
    Utils/pathexpander.hpp \
    SearchPathGens/unicodewhitespacefixer.hpp \
    SearchPathGens/universaljapanesekanalookup.hpp \
    Utils/searchkeys.hpp \
    configmanager.hpp \
    Sys/command.hpp \
    Commands/cmdaudio.hpp \
    Commands/cmdvideo.hpp \
    Commands/cmdmodule.hpp \
    Commands/cmdplaylist.hpp \
    Commands/cmdsearch.hpp \
    Commands/cmdbrowse.hpp \
    Commands/cmdrandom.hpp \
    Commands/cmdshuffle.hpp \
    Commands/cmdrepeat.hpp \
    Commands/cmdhistory.hpp \
    Commands/cmdstatistics.hpp \
    Commands/cmdrescan.hpp \
    Sys/mediaplayercontroller.hpp \
    Sys/kbhit.hpp \
    Sys/playlistparser.hpp \
    Sys/historymanager.hpp \
    SearchPathGens/unicodelatingen.hpp \
    Sys/mediacache.hpp \
    Utils/range_based_for_loop.hpp

##[Local] ─ ignore this; for taglib experiments
#
# notes: try to workaround taglibs unicode madness in RIFF files
#        < TagLib: String::UTF8toUTF16() - Unicode conversion error. >
#        < TagLib: String::UTF8toUTF8() - Unicode conversion error. >
#     results in incomplete (cutoff at first unicode char) or empty strings
#
#debug {
#    LIBS = -L$$PWD/contrib/taglib-1.10/lib \
#           -ltag_c -ltag -lz \
#           -lreadline
#    INCLUDEPATH += $$PWD/contrib/taglib-1.10/include
#}
