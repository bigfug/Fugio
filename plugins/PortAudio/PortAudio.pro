#-------------------------------------------------
#
# Project created by QtCreator 2014-06-27T23:42:17
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += gui widgets

TARGET = $$qtLibraryTarget(fugio-portaudio)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += PORTAUDIO_LIBRARY

SOURCES += \
	portaudiooutputnode.cpp \
	deviceportaudio.cpp \
	portaudioinputnode.cpp \
	portaudioplugin.cpp

HEADERS += \
	../../include/fugio/portaudio/uuid.h \
	portaudiooutputnode.h \
	../../include/fugio/device_factory_interface.h \
	../../include/fugio/nodecontrolbase.h \
	deviceportaudio.h \
	portaudioinputnode.h \
	portaudioplugin.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_portaudio_de.ts \
	translations/fugio_portaudio_es.ts \
	translations/fugio_portaudio_fr.ts

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
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		isEmpty( CASKBASE ) {
			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/Frameworks
			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

windows {
	INSTALLDEST  = $$INSTALLDATA/plugins/portaudio

	plugin.path  = $$INSTALLDEST
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
# portaudio

windows {
	contains( QT_ARCH, x86_64 ) {
		PORTAUDIO_PATH = $$(LIBS)/portaudio.64.2015
		PORTAUDIO_LIB  = portaudio_x64
	} else {
		PORTAUDIO_PATH = $$(LIBS)/portaudio.32.2015
		PORTAUDIO_LIB  = portaudio_x86
	}

	exists( $$(LIBS)/portaudio/include/portaudio.h ) {
		INCLUDEPATH += $$(LIBS)/portaudio/include
	}

	exists( $$PORTAUDIO_PATH ) {
		LIBS += -L$$PORTAUDIO_PATH/Release -l$$PORTAUDIO_LIB

		DEFINES += PORTAUDIO_SUPPORTED

		libraries.path  = $$INSTALLDEST
		libraries.files = $$PORTAUDIO_PATH/Release/$${PORTAUDIO_LIB}.dll

		INSTALLS += libraries
	}
}

macx {
	isEmpty( CASKBASE ) {
		PORTAUDIO_PATH = $$(LIBS)/portaudio-x64

		exists( $$PORTAUDIO_PATH ) {
			INCLUDEPATH += $$PORTAUDIO_PATH/include
			LIBS += -L$$PORTAUDIO_PATH/lib -lportaudio
			DEFINES += PORTAUDIO_SUPPORTED
		}
	} else {
		exists( /usr/local/opt/portaudio ) {
			INCLUDEPATH += /usr/local/opt/portaudio/include
			LIBS += -L/usr/local/opt/portaudio/lib -lportaudio
			DEFINES += PORTAUDIO_SUPPORTED
		}
	}
}

unix:!macx {
	exists( $$[QT_SYSROOT]/usr/include/portaudio.h ) {
#        INCLUDEPATH += $$[QT_SYSROOT]/usr/include

#        LIBS += -lportaudio

#        DEFINES += PORTAUDIO_SUPPORTED

	} else:exists( /usr/local/include/portaudio.h ) {
		LIBS += -lportaudio
		DEFINES += PORTAUDIO_SUPPORTED
	}
}

#unix:!macx:exists( /usr/include/portaudio.h ) {
#    INCLUDEPATH += /usr/include
#    LIBS += -L/usr/lib -lportaudio
#    DEFINES += PORTAUDIO_SUPPORTED
#}

!contains( DEFINES, PORTAUDIO_SUPPORTED ) {
	message( "PortAudio not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include


