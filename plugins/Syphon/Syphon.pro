#-------------------------------------------------
#
# Project created by QtCreator 2016-02-18T15:10:39
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets gui

TARGET = $$qtLibraryTarget(fugio-syphon)
TEMPLATE = lib

CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += SYPHON_LIBRARY

SOURCES += \
	syphonpin.cpp \
	syphonreceivernode.cpp \
	syphonsendernode.cpp \
	syphonplugin.cpp \
	syphonrendernode.cpp

OBJECTIVE_SOURCES += \
	syphonsender.mm \
	syphonreceiver.mm

OBJECTIVE_HEADERS += \
	syphonsender.h \
	syphonreceiver.h

HEADERS += \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/syphon/uuid.h \
	opengl_includes.h \
	syphonpin.h \
	syphonreceivernode.h \
	syphonsendernode.h \
	syphonplugin.h \
	syphonreceiver.h \
	syphonsender.h \
	syphonrendernode.h

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

		QMAKE_POST_LINK += $$libChange( libGLEW.2.0.0.dylib )

		QMAKE_POST_LINK += && rm -rf $$FRAMEWORKDIR/Syphon.framework

		QMAKE_POST_LINK += && cp -R /Library/Frameworks/Syphon.framework $$FRAMEWORKDIR

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
# OpenGL

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	LIBS += -L/opt/vc/lib -lGLESv2 -lEGL
}

win32 {
	INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

	LIBS += -L$$(LIBS)/glew-2.0.0/lib/Release/Win32 -lglew32s

	DEFINES += GLEW_STATIC

	LIBS += -lopengl32
}

macx {
	INCLUDEPATH += /usr/local/include

	LIBS += -L/usr/local/lib -lGLEW
}

#------------------------------------------------------------------------------
# Syphon

macx {
	QMAKE_CFLAGS += -F/Library/Frameworks
	QMAKE_CXXFLAGS += -F/Library/Frameworks

	QMAKE_OBJECTIVE_CFLAGS += -F/Library/Frameworks
	QMAKE_OBJECTIVE_CXXFLAGS += -F/Library/Frameworks

	LIBS += -F/Library/Frameworks

	LIBS += -framework Syphon
	LIBS += -framework OpenGL
	LIBS += -framework Cocoa
	LIBS += -framework Quartz

	DEFINES += SYPHON_SUPPORTED
}

!contains( DEFINES, SYPHON_SUPPORTED ) {
	warning( "Syphon not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

