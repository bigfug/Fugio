#-------------------------------------------------
#
# Project created by QtCreator 2014-07-26T22:35:57
#
#-------------------------------------------------

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-midi)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

CONFIG(debug,debug|release) {
    DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
    DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

SOURCES += \
    midinotenode.cpp \
    midioutputhelpernode.cpp \
    midirotarynode.cpp \
    midiplugin.cpp \
    midiinputhelpernode.cpp \
    notetofrequencynode.cpp \
    frequencytonotenode.cpp \
    scalenode.cpp \
    intervalnode.cpp \
    channeloutputnode.cpp \
    channelinputnode.cpp \
    midiinputpin.cpp \
    midiinputsyncnode.cpp \
    midioutputnode.cpp \
    mididecodernode.cpp \
    midioutputpin.cpp

HEADERS += \
    ../../include/fugio/nodecontrolbase.h \
    ../../include/fugio/pincontrolbase.h \
    ../../include/fugio/midi/midi_input_interface.h \
    ../../include/fugio/midi/midi_interface.h \
    ../../include/fugio/midi/uuid.h \
    midinotenode.h \
    midioutputhelpernode.h \
    midirotarynode.h \
    midiplugin.h \
    midiinputhelpernode.h \
    notetofrequencynode.h \
    frequencytonotenode.h \
    scalenode.h \
    intervalnode.h \
    channeloutputnode.h \
    channelinputnode.h \
    midiinputpin.h \
    midiinputsyncnode.h \
    midioutputnode.h \
    mididecodernode.h \
    midioutputpin.h

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
    DEFINES += TARGET_OS_MAC
    CONFIG -= x86
    CONFIG += lib_bundle

    BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
    INSTALLBASE  = $$OUT_PWD/../../../deploy-installer-$$QMAKE_HOST.arch
    INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
    INSTALLDEST  = $$INSTALLDIR/data/plugins
    INCLUDEDEST  = $$INSTALLDIR/data/include/fugio
    FRAMEWORKDIR = $$BUNDLEDIR/Contents/Frameworks

    DESTDIR = $$BUNDLEDIR/Contents/MacOS
    DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += echo

        LIBCHANGEDEST = $$DESTLIB

        QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
        QMAKE_POST_LINK += $$qtLibChange( QtGui )
        QMAKE_POST_LINK += $$qtLibChange( QtCore )

        QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

        QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
        QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

        QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

        QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
    }
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

