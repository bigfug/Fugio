#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T23:22:43
#
#-------------------------------------------------

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-text)
TEMPLATE = lib
CONFIG += plugin c++11

CONFIG(debug,debug|release) {
		DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
		DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

DEFINES += TEXT_LIBRARY

SOURCES += textplugin.cpp \
	texteditornode.cpp \
	texteditorform.cpp \
	codeeditor.cpp \
	regexpnode.cpp \
	numbertostringnode.cpp \
	stringjoinnode.cpp \
    linebuffernode.cpp

HEADERS += textplugin.h\
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/text/uuid.h \
	../../include/fugio/text/syntax_highlighter_interface.h \
	texteditornode.h \
	texteditorform.h \
	codeeditor.h \
	linenumberarea.h \
	regexpnode.h \
	numbertostringnode.h \
	stringjoinnode.h \
    linebuffernode.h
	cmdtexteditorupdate.h

FORMS += \
	texteditorform.ui

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

#------------------------------------------------------------------------------
# Windows Install

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

