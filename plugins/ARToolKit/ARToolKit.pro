#-------------------------------------------------
#
# Project created by QtCreator 2016-07-23T20:30:54
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets opengl

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
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
	INSTALLDEST  = $$shell_path( $$INSTALLDIR/data/plugins )

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$INSTALLDEST

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$INSTALLDEST
	}
}

#------------------------------------------------------------------------------
# ARToolKit

windows:exists( $$(ARTOOLKIT5_ROOT) ) {
	INCLUDEPATH += $$(ARTOOLKIT5_ROOT)/include

	win32 {
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

	LIBS += -L$$(ARTOOLKIT5_ROOT)/lib -lAR

	LIBS += -framework CoreFoundation -framework AppKit

	DEFINES += ARTOOLKIT_SUPPORTED
}

!contains( DEFINES, ARTOOLKIT_SUPPORTED ) {
	warning( "ARToolKit not supported" )
}

#------------------------------------------------------------------------------
# Lua

win32:exists( $$(LIBS)/Lua-5.3.3 ) {
	INCLUDEPATH += $$(LIBS)/Lua-5.3.3/include

	LIBS += -L$$(LIBS)/Lua-5.3.3 -llua53

	DEFINES += LUA_SUPPORTED
}

macx:exists( /usr/local/include/lua.hpp ) {
	INCLUDEPATH += /usr/local/include

	LIBS += -L/usr/local/lib -llua

	DEFINES += LUA_SUPPORTED
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

