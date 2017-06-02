#-------------------------------------------------
#
# Project created by QtCreator 2014-12-29T21:06:26
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += widgets

TARGET = $$qtLibraryTarget(fugio-vst)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += VST_LIBRARY

SOURCES += vstplugin.cpp \
	usediids.cpp \
	vstview.cpp \
	vst2node.cpp \
	vst3node.cpp

HEADERS += vstplugin.h \
	../../include/fugio/vst/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	vstview.h \
	vst2node.h \
	vst3node.h \
	dirlocker.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_vst_de.ts \
	translations/fugio_vst_es.ts \
	translations/fugio_vst_fr.ts

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

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

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
# portmidi

windows {
	INCLUDEPATH += $$(LIBS)/portmidi/pm_common
	INCLUDEPATH += $$(LIBS)/portmidi/porttime
}

win64 {
	LIBS += -L$$(LIBS)/portmidi.64.2015/Release
}

win32 {
	LIBS += -L$$(LIBS)/portmidi.32.2015/Release
}

macx {
	INCLUDEPATH += /opt/local/include
}

#------------------------------------------------------------------------------
# VST SDK

windows:exists( "$$(LIBS)/VST3SDK" ) {
	INCLUDEPATH += "$$(LIBS)/VST3SDK"

	contains( QT_ARCH, x86_64 ) {
		CONFIG(debug,debug|release) {
			LIBS += -L"$$(LIBS)/VST3SDK/base/win/x64/Debug"
		} else {
			LIBS += -L"$$(LIBS)/VST3SDK/base/win/x64/Release"
		}
	} else {
		CONFIG(debug,debug|release) {
			LIBS += -L"$$(LIBS)/VST3SDK/base/win/Win32/Debug"
		} else {
			LIBS += -L"$$(LIBS)/VST3SDK/base/win/Win32/Release"
		}
	}

	LIBS += -lbase

	LIBS += -lOle32 -lUser32

	LIBS += -llegacy_stdio_definitions

	DEFINES += VST_SUPPORTED

	SOURCES += "$$(LIBS)/VST3SDK/public.sdk/source/common/memorystream.cpp"

	HEADERS += "$$(LIBS)/VST3SDK/public.sdk/source/common/memorystream.h"
}

macx:exists( "$$(LIBS)/VST3 SDK" ) {
	INCLUDEPATH += "$$(LIBS)/VST3 SDK"

	LIBS += -L"$$(LIBS)/VST3 SDK/base/mac"
	LIBS += -lbase_stdc++

	CONFIG(debug,debug|release) {
		DEFINES += _DEBUG
	} else {
		DEFINES += RELEASE
	}

	DEFINES += VST_SUPPORTED

	QMAKE_CFLAGS += -F/Library/Frameworks
	QMAKE_CXXFLAGS += -F/Library/Frameworks

	LIBS += -F/Library/Frameworks

	LIBS += -framework CoreFoundation

	SOURCES += "$$(LIBS)/VST3 SDK/public.sdk/source/common/memorystream.cpp"

	HEADERS += "$$(LIBS)/VST3 SDK/public.sdk/source/common/memorystream.h"
}

linux:!macx:exists( $$(LIBS)/VST3SDK ) {
	INCLUDEPATH += $$(LIBS)/VST3SDK

	DEFINES += __UNIX__ LINUX PTHREADS

#	LIBS += -L"$$(LIBS)/VST3 SDK/base/mac"
#	LIBS += -lbase_stdc++

	CONFIG(debug,debug|release) {
		DEFINES += _DEBUG
	} else {
		DEFINES += RELEASE
	}

	DEFINES += VST_SUPPORTED

	SOURCES += $$(LIBS)/VST3SDK/public.sdk/source/common/memorystream.cpp

	HEADERS += $$(LIBS)/VST3SDK/public.sdk/source/common/memorystream.h
}

!contains( DEFINES, VST_SUPPORTED ) {
	warning( "VST not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
