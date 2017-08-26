#-------------------------------------------------
#
# Project created by QtCreator 2016-07-23T20:30:54
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-artoolkit)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += artoolkitplugin.cpp \
	paramloadnode.cpp \
	parampin.cpp \
	trackernode.cpp \
	paramcameranode.cpp

HEADERS += artoolkitplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/artoolkit/uuid.h \
	../../include/fugio/artoolkit/param_interface.h \
	paramloadnode.h \
	parampin.h \
	trackernode.h \
	paramcameranode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_artoolkit_de.ts \
	translations/fugio_artoolkit_es.ts \
	translations/fugio_artoolkit_fr.ts

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

			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs_shared.sh $$BUNDLEDIR/Contents/MacOS
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

windows {
	plugin.path  = $$INSTALLDATA/plugins
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
    target.path = $$INSTALLBASE/usr/lib/fugio

    INSTALLS += target
}

#------------------------------------------------------------------------------
# ARToolKit

windows:exists( $$(ARTOOLKIT5_ROOT) ) {
	INCLUDEPATH += $$(ARTOOLKIT5_ROOT)/include

	contains( QT_ARCH, x86_64 ) {
		LIBS += -L$$(ARTOOLKIT5_ROOT)/lib/win64-x64
		LIBS += -lshell32
	} else {
		LIBS += -L$$(ARTOOLKIT5_ROOT)/lib/win32-i386
		LIBS += -lshell32
	}

	CONFIG(debug,debug|release) {
		LIBS += -lARd -lARICPd
	} else {
		LIBS += -lAR -lARICP
	}

	LIBS += -llegacy_stdio_definitions

	DEFINES += ARTOOLKIT_SUPPORTED
}

macx:exists( $$(ARTOOLKIT5_ROOT) ) {
	INCLUDEPATH += $$(ARTOOLKIT5_ROOT)/include

	LIBS += -L$$(ARTOOLKIT5_ROOT)/lib -lAR -lARICP

	LIBS += -framework CoreFoundation -framework AppKit -framework Accelerate

	DEFINES += ARTOOLKIT_SUPPORTED
}

!contains( DEFINES, ARTOOLKIT_SUPPORTED ) {
	warning( "ARToolKit not supported" )
}

#------------------------------------------------------------------------------
# Lua

windows {
	contains( QT_ARCH, x86_64 ) {
		LUA_PATH = $$(LIBS)/lua-5.3.3_Win64_dll14_lib
	} else {
		LUA_PATH = $$(LIBS)/Lua-5.3.3
	}

	exists( $$LUA_PATH ) {
		INCLUDEPATH += $$LUA_PATH/include

		LIBS += -L$$LUA_PATH -llua53

		DEFINES += LUA_SUPPORTED
	}
}

macx {
	isEmpty( CASKBASE ) {
		exists( $$(LIBS)/lua-x64 ) {
			INCLUDEPATH += $$(LIBS)/lua-x64/include

			LIBS += -L$$(LIBS)/lua-x64/lib -llua5

			DEFINES += LUA_SUPPORTED
		}
	} else {
		exists( /usr/local/opt/lua ) {
			INCLUDEPATH += /usr/local/opt/lua/include

			LIBS += -L/usr/local/opt/lua/lib -llua

			DEFINES += LUA_SUPPORTED
		}
	}
}

unix:!macx {
	exists( $$[QT_SYSROOT]/usr/include/lua5.2/lua.h ) {
		INCLUDEPATH += $$[QT_SYSROOT]/usr/include/lua5.2

		LIBS += -llua5.2

		DEFINES += LUA_SUPPORTED

	} else:exists( /usr/include/lua5.3/lua.h ) {
		INCLUDEPATH += /usr/include/lua5.3

		LIBS += -llua5.3

		DEFINES += LUA_SUPPORTED

	} else:exists( /usr/include/lua5.2/lua.h ) {
		INCLUDEPATH += /usr/include/lua5.2

		LIBS += -llua5.2

		DEFINES += LUA_SUPPORTED
	}
}

!contains( DEFINES, LUA_SUPPORTED ) {
	warning( "Lua not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

