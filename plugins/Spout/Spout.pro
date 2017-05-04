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

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_spout_de.ts \
	translations/fugio_spout_es.ts \
	translations/fugio_spout_fr.ts

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

	isEmpty( CASKBASE ) {
		libraries.path  = $$DESTDIR/../libs
		libraries.files = $$(LIBS)/glew-2.0.0/lib/libGLEW.2.0.0.dylib

		INSTALLS += libraries
	}

	DESTDIR = $$BUNDLEDIR/Contents/MacOS
	DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		isEmpty( CASKBASE ) {
			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs_shared.sh $$BUNDLEDIR/Contents/MacOS
		}

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

windows {
	INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

	contains( QT_ARCH, x86_64 ) {
		GLEW_PATH = $$(LIBS)/glew.64.2015
	} else {
		GLEW_PATH = $$(LIBS)/glew.32.2015
	}

	CONFIG(release,debug|release) {
		GLEW_PATH = $$GLEW_PATH/lib/Release
	} else {
		GLEW_PATH = $$GLEW_PATH/lib/Debug
	}

	exists( $$GLEW_PATH ) {
		LIBS += -L$$GLEW_PATH -llibglew32

		DEFINES += GLEW_STATIC

		LIBS += -lopengl32
	}
}

macx {
	isEmpty( CASKBASE ) {
		INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

		LIBS += -L$$(LIBS)/glew-2.0.0/lib -lGLEW
	} else {
		INCLUDEPATH += /usr/local/include

		LIBS += -L/usr/local/lib -lGLEW
	}
}

#------------------------------------------------------------------------------
# Spout

win32 {
	exists( $$(LIBS)/Spout2/SpoutSDK/Source ) {
		SOURCES += \
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutCopy.cpp \
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
			$$(LIBS)/Spout2/SpoutSDK/Source/SpoutCopy.h \
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
