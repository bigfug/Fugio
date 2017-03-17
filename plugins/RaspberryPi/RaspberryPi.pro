#-------------------------------------------------
#
# Project created by QtCreator 2015-12-30T13:25:41
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       -= gui

TARGET = $$qtLibraryTarget(fugio-raspberrypi)
TEMPLATE = lib

CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += raspberrypiplugin.cpp

HEADERS += raspberrypiplugin.h \
	../../include/fugio/raspberrypi/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
	DEFINES += TARGET_OS_MAC
	CONFIG -= x86
	CONFIG += lib_bundle

	BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
	INSTALLDEST  = $$INSTALLDATA/plugins
	INCLUDEDEST  = $$INSTALLDATA/include/fugio
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

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

#------------------------------------------------------------------------------
# Windows Install

windows {
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )
	}
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
	INSTALLDIR = $$INSTALLBASE/packages/com.bigfug.fugio

	contains( DEFINES, Q_OS_RASPBERRY_PI ) {
		target.path = Desktop/Fugio/plugins
	} else {
		target.path = $$shell_path( $$INSTALLDIR/data/plugins )
	}

	INSTALLS += target
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------
# Raspberry Pi

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	INCLUDEPATH += $$[QT_SYSROOT]/opt/vc/include $$[QT_SYSROOT]/opt/vc/include/interface/vcos/pthreads $$[QT_SYSROOT]/opt/vc/include/interface/vmcs_host/linux

	LIBS += -L$$[QT_SYSROOT]/opt/vc/lib -lbcm_host
}

#------------------------------------------------------------------------------
# pigpio

unix:exists( $$[QT_SYSROOT]/usr/local/include/pigpio.h ) {
	INCLUDEPATH += $$[QT_SYSROOT]/usr/local/include
	LIBS += -L$$[QT_SYSROOT]/usr/local/lib -lpigpiod_if2
	DEFINES += PIGPIO_SUPPORTED
}

!contains( DEFINES, PIGPIO_SUPPORTED ) {
	message( "pigpio not supported" )
}
