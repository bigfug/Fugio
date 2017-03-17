#-------------------------------------------------
#
# Project created by QtCreator 2015-01-02T14:40:26
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += gui widgets

TARGET = $$qtLibraryTarget(fugio-audio)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += AUDIO_LIBRARY

SOURCES += audioplugin.cpp \
	audiomixernode.cpp \
	audiopin.cpp \
	fftpin.cpp \
	signalnode.cpp \
	centroidnode.cpp \
	magnitudenode.cpp \
	frequencybandsnode.cpp \
	audiofilternode.cpp \
	audiofilterdisplay.cpp \
	vcfnode.cpp \
	mononode.cpp \
	ringmodulatornode.cpp

HEADERS += audioplugin.h\
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/audio/uuid.h \
	../../include/fugio/audio/audio_producer_interface.h \
	../../include/fugio/audio/audio_sample_format.h \
	../../include/fugio/audio/fft_interface.h \
	audiomixernode.h \
	audiopin.h \
	fftpin.h \
	signalnode.h \
	centroidnode.h \
	magnitudenode.h \
	frequencybandsnode.h \
	audiofilternode.h \
	audiofilterdisplay.h \
	vcfnode.h \
	mononode.h \
	../../include/fugio/audio/audio_instance_base.h \
	ringmodulatornode.h

TRANSLATIONS = \
	$$FUGIO_BASE/translations/fugio_audio_de.ts \
	$$FUGIO_BASE/translations/fugio_audio_es.ts \
	$$FUGIO_BASE/translations/fugio_audio_fr.ts

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
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

windows {
	INSTALLDIR = $$INSTALLBASE/packages/com.bigfug.fugio

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
# API

INCLUDEPATH += $$PWD/../../include

