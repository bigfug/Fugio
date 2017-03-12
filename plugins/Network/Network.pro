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
	websocketclientnode.cpp

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
	websocketclientnode.h

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

		QMAKE_POST_LINK += & for %I in ( $$shell_path( $(QTDIR)/bin/Qt5WebSockets.dll ) ) do copy %I $$shell_path( $$INSTALLDIR/data/ )
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
