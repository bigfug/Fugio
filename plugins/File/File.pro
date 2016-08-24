#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T23:37:33
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-file)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += FILE_LIBRARY

SOURCES += fileplugin.cpp \
	filewatchernode.cpp \
	filenamepin.cpp \
	filenamenode.cpp \
	loadnode.cpp

HEADERS += fileplugin.h\
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	filewatchernode.h \
	filenamepin.h \
	filenamenode.h \
	../../include/fugio/file/uuid.h \
	../../include/fugio/file/filename_interface.h \
	loadnode.h

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
# Windows

windows {
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
	INSTALLDEST  = $$INSTALLDIR/data/plugins

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

