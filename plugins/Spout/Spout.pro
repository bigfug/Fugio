#-------------------------------------------------
#
# Project created by QtCreator 2015-07-04T15:15:57
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets gui

TARGET = $$qtLibraryTarget(fugio-spout)
TEMPLATE = lib

CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += SPOUT_LIBRARY

SOURCES += spoutplugin.cpp \
	spoutreceivernode.cpp \
	spoutpin.cpp \
	spoutreceiverform.cpp \
	spoutsendernode.cpp

HEADERS += spoutplugin.h\
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/spout/uuid.h \
	spoutreceivernode.h \
	spoutpin.h \
	spoutreceiverform.h \
	spoutsendernode.h \
	opengl_includes.h

FORMS += \
	spoutreceiverform.ui

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

macx:exists( /usr/local/opt/glew ) {
	INCLUDEPATH += /usr/local/include

	LIBS += -L/usr/local/lib -lGLEW
}

mac:exists( $$(LIBS)/glew-2.0.0 ) {
	INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

	LIBS += $$(LIBS)/glew-2.0.0/lib/libGLEW.a

	DEFINES += GLEW_STATIC
}

#------------------------------------------------------------------------------
# Spout

win32 {
	exists( $$(LIBS)/Spout2/SpoutSDK/Source ) {
		SOURCES += \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutDirectX.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutGLDXinterop.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutGLextensions.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutMemoryShare.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutReceiver.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSDK.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSender.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSenderMemory.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSenderNames.cpp \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSharedMemory.cpp

		HEADERS += \
			$$(LIBS)/Spout2/SpoutSDK/Source/Spout.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutCommon.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutDirectX.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutGLDXinterop.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutGLextensions.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutMemoryShare.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutReceiver.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSDK.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSender.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSenderMemory.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSenderNames.h \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutSharedMemory.h

		INCLUDEPATH += $$(LIBS)/Spout2/SpoutSDK/Source

		LIBS += -luser32 -lopengl32 -lGdi32

		DEFINES += SPOUT_SUPPORTED
	}
}

!contains( DEFINES, SPOUT_SUPPORTED ) {
		warning( "Spout not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
