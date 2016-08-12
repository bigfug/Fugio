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

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
    DEFINES += TARGET_OS_MAC
    CONFIG -= x86
    CONFIG += lib_bundle

    BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
        INSTALLBASE  = $$FUGIO_ROOT/deploy-installer-$$QMAKE_HOST.arch
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

        QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite

        QMAKE_POST_LINK += $$libChange( libportaudio.2.dylib )

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
        QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

        QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

        QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
    }
}

windows {
        INSTALLBASE  = $$FUGIO_ROOT/deploy-installer-$$QMAKE_HOST.arch
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins/portaudio )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins/portaudio )

		win32 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/portaudio.32.2013/bin/Win32/Release/portaudio_x86.dll ) $$shell_path( $$INSTALLDIR/data/plugins/portaudio )
		}

		win64 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/portaudio.64.2013/bin/x64/Release/portaudio_x64.dll ) $$shell_path( $$INSTALLDIR/data/plugins/portaudio )
		}
	}
}

#------------------------------------------------------------------------------
# portaudio

windows {
    contains( QMAKE_HOST.arch, x86_64 ) {
        exists( $$(LIBS)/portaudio/include/portaudio.h ) {
            LIBS += -L$$(LIBS)/portaudio.64.2013/bin/x64/Release
            INCLUDEPATH += $$(LIBS)/portaudio/include
            LIBS += -lportaudio_x64
            DEFINES += PORTAUDIO_SUPPORTED
        }
    } else {
        exists( $$(LIBS)/portaudio/include/portaudio.h ) {
            LIBS += -L$$(LIBS)/portaudio.32.2013/bin/Win32/Release
            INCLUDEPATH += $$(LIBS)/portaudio/include
            LIBS += -lportaudio_x86
            DEFINES += PORTAUDIO_SUPPORTED
        }
    }
}

macx:exists( /usr/local/include/portaudio.h ) {
    INCLUDEPATH += /usr/local/include
    LIBS += /usr/local/lib/libportaudio.a
    LIBS += -framework Carbon -framework AudioUnit -framework AudioToolbox -framework CoreAudio
    DEFINES += PORTAUDIO_SUPPORTED
}

unix:!macx:exists( /usr/local/include/portaudio.h ) {
    LIBS += -lportaudio
    DEFINES += PORTAUDIO_SUPPORTED
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


