#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T09:48:22
#
#-------------------------------------------------

QT += gui widgets

TARGET = $$qtLibraryTarget(fugio-core)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

CONFIG(debug,debug|release) {
	DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
	DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

DEFINES += BASENODES_LIBRARY

SOURCES += \
    stringpin.cpp \
    integerpin.cpp \
    floatpin.cpp \
    triggerpin.cpp \
    boolpin.cpp \
    bytearraypin.cpp \
    numberrangenode.cpp \
    randomnumbernode.cpp \
    anytriggernode.cpp \
    ratecontrolnode.cpp \
    numberrangelimitnode.cpp \
    boolrangenode.cpp \
    togglenode.cpp \
    triggerrangenumbernode.cpp \
    outputrangenode.cpp \
    sizepin.cpp \
    splitsizenode.cpp \
    joinsizenode.cpp \
    arraypin.cpp \
    pointpin.cpp \
    fadenode.cpp \
    size3dpin.cpp \
    coreplugin.cpp \
    splitlistnode.cpp \
    listpin.cpp \
    indexnode.cpp \
    booltotriggernode.cpp \
    signalnumbernode.cpp \
    envelopenode.cpp \
    numberspreadnode.cpp \
    choicepin.cpp \
    splitpointnode.cpp \
    variantpin.cpp \
    duplicatepinsnode.cpp \
    lastupdatedinputnode.cpp \
    counternode.cpp \
    stringlistpin.cpp \
    listsizenode.cpp \
    listindexnode.cpp \
    framedelaynode.cpp \
    typesizenode.cpp \
    bytearraylistpin.cpp \
    switchnode.cpp

HEADERS += \
    stringpin.h \
    integerpin.h \
    floatpin.h \
    ../../include/fugio/core/uuid.h \
    ../../include/fugio/playhead_interface.h \
    triggerpin.h \
    boolpin.h \
    ../../include/fugio/nodecontrolbase.h \
    ../../include/fugio/pincontrolbase.h \
    bytearraypin.h \
    numberrangenode.h \
    randomnumbernode.h \
    anytriggernode.h \
    ratecontrolnode.h \
    numberrangelimitnode.h \
    boolrangenode.h \
    togglenode.h \
    triggerrangenumbernode.h \
    outputrangenode.h \
    sizepin.h \
    splitsizenode.h \
    joinsizenode.h \
    arraypin.h \
    pointpin.h \
    fadenode.h \
    size3dpin.h \
    coreplugin.h \
    splitlistnode.h \
    listpin.h \
    indexnode.h \
    booltotriggernode.h \
    signalnumbernode.h \
    envelopenode.h \
    numberspreadnode.h \
    ../../include/fugio/core/variant_interface.h \
    ../../include/fugio/core/array_interface.h \
    ../../include/fugio/core/list_interface.h \
    ../../include/fugio/core/struct_entry_interface.h \
    choicepin.h \
    splitpointnode.h \
    variantpin.h \
    duplicatepinsnode.h \
    lastupdatedinputnode.h \
    counternode.h \
    stringlistpin.h \
    listsizenode.h \
    listindexnode.h \
    framedelaynode.h \
    typesizenode.h \
    bytearraylistpin.h \
    switchnode.h

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

    DESTDIR = $$BUNDLEDIR/Contents/MacOS
    DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += echo

        LIBCHANGEDEST = $$DESTLIB

        QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
        QMAKE_POST_LINK += $$qtLibChange( QtGui )
        QMAKE_POST_LINK += $$qtLibChange( QtCore )

        QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

        QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
        QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

        QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

        QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
    }
}

windows {
    INSTALLBASE  = $$OUT_PWD/../../../deploy-installer-$$QMAKE_HOST.arch
    INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += echo

        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )
    }
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
