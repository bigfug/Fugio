#-------------------------------------------------
#
# Project created by QtCreator 2016-03-03T15:40:50
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

TARGET = $$qtLibraryTarget(fugio-network)
TEMPLATE = lib

CONFIG += plugin c++11

QT += gui widgets network

qtHaveModule( websockets ) {
	QT += websockets

	DEFINES += WEBSOCKET_SUPPORTED
}

DESTDIR = $$DESTDIR/plugins

DEFINES += NETWORK_LIBRARY

SOURCES += networkplugin.cpp \
	tcpsendnode.cpp \
	tcpreceivenode.cpp \
	tcpreceiverawnode.cpp \
	tcpsendrawnode.cpp \
	udpreceiverawnode.cpp \
	udpsendrawnode.cpp \
	slipencodenode.cpp \
	slipdecodenode.cpp \
	getnode.cpp \
	websocketdataservernode.cpp \
	cobsdecodenode.cpp \
	cobsencodenode.cpp \
	websocketclientnode.cpp \
	packetencodenode.cpp \
	crc32.cpp \
	packetdecodenode.cpp \
    universesendnode.cpp \
    universereceivenode.cpp

HEADERS += networkplugin.h\
	../../include/fugio/network/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	tcpsendnode.h \
	tcpreceivenode.h \
	tcpreceiverawnode.h \
	tcpsendrawnode.h \
	udpreceiverawnode.h \
	udpsendrawnode.h \
	slipencodenode.h \
	slipdecodenode.h \
	getnode.h \
	websocketdataservernode.h \
	cobsdecodenode.h \
	cobsencodenode.h \
	websocketclientnode.h \
	packetencodenode.h \
	crc32.h \
	packetdecodenode.h \
    universesendnode.h \
    universereceivenode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_network_de.ts \
	translations/fugio_network_es.ts \
	translations/fugio_network_fr.ts

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
	DEFINES += TARGET_OS_MAC
	CONFIG -= x86
	CONFIG += lib_bundle

	BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
	INSTALLDEST  = $$INSTALLDATA/plugins
	INCLUDEDEST  = $$INSTALLDATA/include/fugio

	DESTDIR = $$BUNDLEDIR/Contents/MacOS
	DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtNetwork )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )
		QMAKE_POST_LINK += $$qtLibChange( QtWebSockets )

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
	plugin.path  = $$INSTALLDATA/plugins
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin

	libraries.path  = $$INSTALLDATA
	libraries.files = $$(QTDIR)/bin/Qt5Network.dll $$(QTDIR)/bin/Qt5WebSockets.dll

	INSTALLS += libraries
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
    target.path = $$INSTALLBASE/usr/lib/fugio

    INSTALLS += target
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
