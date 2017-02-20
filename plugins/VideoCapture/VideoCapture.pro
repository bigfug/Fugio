#-------------------------------------------------
#
# Project created by QtCreator 2017-02-20T11:12:52
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-videocapture)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += videocaptureplugin.cpp \
	videocapturenode.cpp

HEADERS += videocaptureplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/videocapture/uuid.h \
	videocapturenode.h

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

	CONFIG(debug,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB
	}

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$FRAMEWORKDIR
		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS

		QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
		QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
		QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

		QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

		QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
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
# dlib

macx:exists( $$(LIBS)/video_capture/install/mac-clang-x86_64/include/videocapture/Capture.h ) {
	INCLUDEPATH += $$(LIBS)/video_capture/install/mac-clang-x86_64/include

	LIBS += -L$$(LIBS)/video_capture/install/mac-clang-x86_64/lib -lvideocapture

	LIBS += -framework Cocoa -framework Carbon -framework CoreMedia -framework CoreVideo -framework AVFoundation

	DEFINES += VIDEOCAPTURE_SUPPORTED
}

win32:exists( $$(LIBS)/video_capture/build ) {
	INCLUDEPATH += $$(LIBS)/video_capture/include

	CONFIG(debug,debug|release) {
		LIBS += -L$$(LIBS)/video_capture/build/Debug
	} else {
		LIBS += -L$$(LIBS)/video_capture/build/Release
	}

	LIBS += -lvideocapture

	LIBS += -lShlwapi -lOle32 -lMfplat -lMf -lMfuuid -lMfreadwrite

	DEFINES += VIDEOCAPTURE_SUPPORTED
}

!contains( DEFINES, VIDEOCAPTURE_SUPPORTED ) {
	warning( "videocapture not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
