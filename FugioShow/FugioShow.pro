#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T18:56:36
#
#-------------------------------------------------

include( ../FugioGlobal.pri )

QT += core network # gui
#QT -= gui
#QT += widgets concurrent network

greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets

unix:!macx: {
    TARGET = fugioshow
} else {
    TARGET = FugioShow
}

TEMPLATE = app
CONFIG += console

#-------------------------------------------------
# Read in the Fugio version from version.txt

FUGIO_VERSION = "$$cat(version.txt)"

DEFINES += "FUGIO_VERSION=\"$$FUGIO_VERSION\""

#-------------------------------------------------

SOURCES += main.cpp \
    show.cpp

HEADERS += show.h

#-------------------------------------------------

windows {
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

linux:!macx {
    QMAKE_LFLAGS += -R '\\\$\\\$ORIGIN'
}

macx {
#	QMAKE_INFO_PLIST = Info.plist

#	APP_DIR      = $$DESTDIR/$$TARGET".app"
#	PLUGIN_DIR   = $$APP_DIR/Contents/PlugIns
#	RESOURCE_DIR = $$APP_DIR/Contents/Resources

#	QMAKE_POST_LINK += echo

	CONFIG(release,debug|release) {
#		QMAKE_POST_LINK += && macdeployqt $$APP_DIR -always-overwrite -qmldir=$$shell_path( $$FUGIO_BASE/qml )

#		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$APP_DIR ) CFBundleVersion \"$$FUGIO_VERSION\"
#		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$APP_DIR ) CFBundleGetInfoString \"$$FUGIO_VERSION\"

#		QMAKE_POST_LINK += && install_name_tool -add_rpath @loader_path/../../../libs $$APP_DIR/Contents/MacOS/FugioShow

#		isEmpty( CASKBASE ) {
#			installer_meta.path  = $$INSTALLROOT/meta
#			installer_meta.files = $$_PRO_FILE_PWD_/package.xml

#			installer_config.path  = $$INSTALLBASE/config
#			installer_config.extra = cp $$_PRO_FILE_PWD_/../config.osx.xml $$installer_config.path/config.xml

#			INSTALLS += installer_meta installer_config
#		}

		app.path  = $$INSTALLDATA
		app.files = $$APP_DIR
#		app.extra = cp $$(QTDIR)/plugins/platforms/libqcocoa.dylib $$PLUGIN_DIR/platforms

		INSTALLS += app
	}

#	isEmpty( CASKBASE ) {
#		brew_meta.path = $$INSTALLBASE/packages/sh.brew/meta
#		brew_meta.files = ../installer/package.xml ../installer/installscript.qs

#		brew_data.path = $$INSTALLBASE/packages/sh.brew/data
#		brew_data.files = ../installer/brew_install_update

#		INSTALLS += brew_meta brew_data
#	}

#	qttranslation.path = $$app.path/$$TARGET".app"/Contents/translations
#	qttranslation.files = $$(QTDIR)/translations/qt*.qm

#	INSTALLS += qttranslation
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../include

#win32:CONFIG(release, debug|release): LIBS += -L$$DESTDIR -lfugio
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR -lfugiod
#else:unix:CONFIG(release, debug|release): LIBS += -L$$DESTDIR -lfugio
#else:unix:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR -lfugio_debug

LIBS += -L$$DESTDIR -l$$qtLibraryTarget( fugio )

INCLUDEPATH += $$PWD/../FugioLib
DEPENDPATH += $$PWD/../FugioLib

win32: PRE_TARGETDEPS += $$DESTDIR/$$qtLibraryTarget( fugio ).lib
else:  PRE_TARGETDEPS += $$DESTDIR/lib$$qtLibraryTarget( fugio ).a

#------------------------------------------------------------------------------
# Python3

win32:CONFIG(release,debug|release) {
   LIBS += -LC:/Python34/DLLs
} else:win32:CONFIG(debug,debug|release) {
    LIBS += -L$$(LIBS)/Python-3.4.3/PCbuild
}

#------------------------------------------------------------------------------
# Assimp

win32 {
	CONFIG(debug,debug|release) {
		LIBS += -L$$(LIBS)/assimp-32/code/Debug
	} else {
		LIBS += -L$$(LIBS)/assimp-32/code/Release
	}
}

#------------------------------------------------------------------------------
# portaudio

win32 {
		LIBS += -L$$(LIBS)/portaudio.32.2013/bin/Win32/Release
}

#------------------------------------------------------------------------------
# OpenCV

win32 {
		LIBS += -L$$(LIBS)/opencv-2.4.9/build/x86/vc12/bin
}

#------------------------------------------------------------------------------
# ffmpeg

unix:!macx {
	LIBS += -L/usr/local/lib
}

win32 {
	LIBS += -L$$(LIBS)/ffmpeg-2.8.5-32/lib
	LIBS += -L$$(LIBS)/ffmpeg-2.8.5-32/bin
}

macx {
	LIBS += -L/opt/local/lib
}

#------------------------------------------------------------------------------
# portmidi

windows {
		LIBS += -L$$(LIBS)/portmidi.32.2013/Release
}

#------------------------------------------------------------------------------
# Leap

win32 {
	LIBS += -L$$(LIBS)/LeapSDK/lib/x86
}

#------------------------------------------------------------------------------
# fugStream

win32 {
	CONFIG(debug,debug|release) {
		LIBS += -L../../../freeframe/freeframe-debug
	} else {
		LIBS += -L../../../freeframe/freeframe-release
	}
}

#------------------------------------------------------------------------------
# fugStream

win32 {
	LIBS += -L$$(LIBS)/fftw-3.3.4
}

#------------------------------------------------------------------------------
# Oculus Rift

win32 {
	exists( $$(LIBS)/OculusSDK-0.8.0/LibOVR ) {
		#INCLUDEPATH += $$(LIBS)/OculusSDK/LibOVR/Include

		#LIBS += -L$$(LIBS)/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2012 -lLibOVR
	}
}

#------------------------------------------------------------------------------
# Bullet Physics

win32 {
#    LIBS += -L$$(LIBS)/bullet3-2.83.4/bin
}
