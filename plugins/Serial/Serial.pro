#-------------------------------------------------
#
# Project created by QtCreator 2014-07-30T08:45:54
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += widgets

qtHaveModule( serialport ) {
	QT += serialport

	DEFINES += SERIALPORT_SUPPORTED
}

TARGET = $$qtLibraryTarget(fugio-serial)
TEMPLATE = lib
CONFIG += plugin c++11
DESTDIR = $$DESTDIR/plugins

SOURCES += \
	deviceserial.cpp \
	serialinputnode.cpp \
	deviceserialconfiguration.cpp \
	serialoutputnode.cpp \
	serialplugin.cpp \
    serialencodernode.cpp \
    serialdecodernode.cpp

HEADERS += \
	../../include/fugio/serial/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	deviceserial.h \
	serialinputnode.h \
	deviceserialconfiguration.h \
	serialoutputnode.h \
	serialplugin.h \
    serialencodernode.h \
    serialdecodernode.h

FORMS += \
	deviceserialconfiguration.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_serial_de.ts \
	translations/fugio_serial_es.ts \
	translations/fugio_serial_fr.ts

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
		QMAKE_POST_LINK += $$qtLibChange( QtSerialPort )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}


windows {
	plugin.path  = $$INSTALLDATA/plugins
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin

	libraries.path  = $$INSTALLDATA
	libraries.files = $$(QTDIR)/bin/Qt5SerialPort.dll

	INSTALLS += libraries
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

