#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T21:55:17
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-leapmotion)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += LEAPMOTION_LIBRARY

SOURCES += leapmotionplugin.cpp \
	deviceleap.cpp \
	leapnode.cpp \
	leaprectifynode.cpp \
	leaphandnode.cpp \
	leaphandpin.cpp

HEADERS += leapmotionplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/leapmotion/uuid.h \
	../../include/fugio/leapmotion/hand_interface.h \
	deviceleap.h \
	leapnode.h \
	leaprectifynode.h \
	leaphandnode.h \
	leaphandpin.h

TRANSLATIONS = \
	$$FUGIO_BASE/translations/fugio_leapmotion_de.ts \
	$$FUGIO_BASE/translations/fugio_leapmotion_es.ts \
	$$FUGIO_BASE/translations/fugio_leapmotion_fr.ts

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

		QMAKE_POST_LINK += && install_name_tool -change @loader_path/libLeap.dylib $$(LIBS)/LeapSDK/lib/libLeap.dylib $$DESTDIR/libfugio-leapmotion.dylib

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

		QMAKE_POST_LINK += && install_name_tool -change $$(LIBS)/LeapSDK/lib/libLeap.dylib @loader_path/../Frameworks/libLeap.dylib $$DESTDIR/libfugio-leapmotion.dylib

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

windows {
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins/leapmotion )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins/leapmotion )

		win32 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/LeapSDK/lib/x86/Leap.dll ) $$shell_path( $$INSTALLDIR/data/plugins/leapmotion )
		}

		win64 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/LeapSDK/lib/x64/Leap.dll ) $$shell_path( $$INSTALLDIR/data/plugins/leapmotion )
		}
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------

windows:contains(QMAKE_CC, cl):exists( $$(LIBS)/LeapSDK ) {
	INCLUDEPATH += $$(LIBS)/LeapSDK/include

	win32 {
		LIBS += -L$$(LIBS)/LeapSDK/lib/x86 -lLeap
	}

	win64 {
		LIBS += -L$$(LIBS)/LeapSDK/lib/x64 -lLeap
	}

	DEFINES += LEAP_PLUGIN_SUPPORTED

	DEFINES += _SCL_SECURE_NO_WARNINGS
}

macx:exists( $$(LIBS)/LeapSDK ) {
	INCLUDEPATH += $$(LIBS)/LeapSDK/include

	LIBS += -L$$(LIBS)/LeapSDK/lib -lLeap

	DEFINES += LEAP_PLUGIN_SUPPORTED

	CONFIG(debug,debug|release) {
		QMAKE_POST_LINK += install_name_tool -change @loader_path/libLeap.dylib $$(LIBS)/LeapSDK/lib/libLeap.dylib $$DESTDIR/libfugio-leapmotion_debug.dylib
	} else {
#        QMAKE_POST_LINK += && install_name_tool -change @loader_path/libLeap.dylib $$(LIBS)/LeapSDK/lib/libLeap.dylib $$DESTDIR/libfugio-leap.dylib
	}
}

!contains( DEFINES, LEAP_PLUGIN_SUPPORTED ) {
	warning( "Leap Motion not supported" )
}

