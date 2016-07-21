#-------------------------------------------------
#
# Project created by QtCreator 2015-01-31T12:12:25
#
#-------------------------------------------------

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-time)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

CONFIG(debug,debug|release) {
	DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
	DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

DEFINES += TIME_LIBRARY

SOURCES += timeplugin.cpp \
    clocknode.cpp \
    cronnode.cpp \
    everynode.cpp \
    delaynode.cpp \
    inertianode.cpp \
    playheadnode.cpp \
    beattapnode.cpp \
    playheadcontrolnode.cpp

HEADERS += timeplugin.h \
    ../../include/fugio/time/uuid.h \
    ../../include/fugio/nodecontrolbase.h \
    ../../include/fugio/pincontrolbase.h \
    clocknode.h \
    cronnode.h \
    everynode.h \
    delaynode.h \
    inertianode.h \
    playheadnode.h \
    beattapnode.h \
    playheadcontrolnode.h

FORMS += \
    cronform.ui

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

        QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

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
