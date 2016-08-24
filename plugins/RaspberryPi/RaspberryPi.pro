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
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------
# Raspberry Pi

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	INCLUDEPATH += /opt/vc/include /opt/vc/include/interface/vcos/pthreads /opt/vc/include/interface/vmcs_host/linux

	LIBS += -L/opt/vc/lib -lbcm_host
}

#------------------------------------------------------------------------------
# Wiring

unix:exists( /usr/local/include/wiringPi.h ) {
	INCLUDEPATH += /usr/local/include
	LIBS += -L/usr/local/lib -lwiringPi
	DEFINES += WIRINGPI_SUPPORTED
}

!contains( DEFINES, WIRINGPI_SUPPORTED ) {
	message( "Wiring Pi not supported" )
}
