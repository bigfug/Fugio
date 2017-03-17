#-------------------------------------------------
#
# Project created by QtCreator 2014-07-26T22:35:57
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-portmidi)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	devicemidi.cpp \
	portmidiplugin.cpp \
	portmidiinputnode.cpp \
	portmidioutputnode.cpp

HEADERS += \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/portmidi/uuid.h \
	devicemidi.h \
	portmidiplugin.h \
	portmidiinputnode.h \
	portmidioutputnode.h

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
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
	INSTALLDEST  = $$INSTALLDIR/data/plugins/portmidi

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

		win32 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/portmidi.32.2015/Release/portmidi.dll ) $$shell_path( $$INSTALLDEST )
		}

		win64 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/portmidi.64.2015/Release/portmidi.dll ) $$shell_path( $$INSTALLDEST )
		}
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
# portmidi

windows:exists( $$(LIBS)/portmidi ) {
	INCLUDEPATH += $$(LIBS)/portmidi/pm_common
	INCLUDEPATH += $$(LIBS)/portmidi/porttime
}

win64:exists( $$(LIBS)/portmidi.64.2015 ) {
	LIBS += -L$$(LIBS)/portmidi.64.2015/Release -lportmidi

	DEFINES += PORTMIDI_SUPPORTED
}

win32:exists( $$(LIBS)/portmidi.32.2015 )  {
	LIBS += -L$$(LIBS)/portmidi.32.2015/Release -lportmidi

	DEFINES += PORTMIDI_SUPPORTED
}

macx {
	exists( /usr/local/opt/portmidi ) {
		INCLUDEPATH += /usr/local/opt/portmidi/include
		LIBS += -L/usr/local/opt/portmidi/lib -lportmidi
		DEFINES += PORTMIDI_SUPPORTED
	} else:exists( $$(LIBS)/portmidi ) {
		INCLUDEPATH += $$(LIBS)/portmidi/pm_common
		LIBS += $$(LIBS)/portmidi/libportmidi_s.a
		LIBS += -framework Carbon -framework AudioUnit -framework AudioToolbox -framework CoreAudio -framework CoreMIDI
		DEFINES += PORTMIDI_SUPPORTED
	}
}

unix:!macx {
	exists( $$[QT_SYSROOT]/usr/include/portmidi.h ) {
		INCLUDEPATH += $$[QT_SYSROOT]/usr/include
		LIBS += -L$$[QT_SYSROOT]/usr/lib -lportmidi
		DEFINES += PORTMIDI_SUPPORTED

	} else:exists( /usr/include/portmidi.h ) {
		INCLUDEPATH += /usr/include
		LIBS += -L/usr/lib -lportmidi
		DEFINES += PORTMIDI_SUPPORTED
	}
}

!contains( DEFINES, PORTMIDI_SUPPORTED ) {
	message( "PortMidi not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

