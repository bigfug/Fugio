#-------------------------------------------------
#
# Project created by QtCreator 2015-01-26T00:59:00
#
#-------------------------------------------------

include( ../../Fugio/FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-painter)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	painterplugin.cpp \
	painternode.cpp \
    circlenode.cpp \
    clearnode.cpp \
    drawimagenode.cpp \
    rectnode.cpp \
	pennode.cpp \
    brushnode.cpp \
    textnode.cpp \
    switchnode.cpp

HEADERS += \
	painterplugin.h \
	../include/fugio/painter/uuid.h \
	../../Fugio/include/fugio/nodecontrolbase.h \
	../../Fugio/include/fugio/pincontrolbase.h \
	painternode.h \
    circlenode.h \
    clearnode.h \
    drawimagenode.h \
    rectnode.h \
	pennode.h \
	../include/fugio/painter/painter_helper.h \
    brushnode.h \
    textnode.h \
    switchnode.h

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../include
INCLUDEPATH += $$PWD/../../Fugio/include

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

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR
	}
}
