#-------------------------------------------------
#
# Project created by QtCreator 2014-07-30T08:45:54
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets serialport

TARGET = $$qtLibraryTarget(fugio-serial)
TEMPLATE = lib
CONFIG += plugin c++11
DESTDIR = $$DESTDIR/plugins

SOURCES += \
	deviceserial.cpp \
	serialinputnode.cpp \
	deviceserialconfiguration.cpp \
	serialoutputnode.cpp \
	serialplugin.cpp

HEADERS += \
	../../include/fugio/serial/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	deviceserial.h \
	serialinputnode.h \
	deviceserialconfiguration.h \
	serialoutputnode.h \
	serialplugin.h

FORMS += \
	deviceserialconfiguration.ui

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
		QMAKE_POST_LINK += $$qtLibChange( QtSerialPort )

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
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & for %I in ( $$shell_path( $(QTDIR)/bin/Qt5SerialPort.dll ) ) do copy %I $$shell_path( $$INSTALLDIR/data/ )

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

