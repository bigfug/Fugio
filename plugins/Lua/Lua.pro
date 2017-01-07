#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T09:23:05
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

TARGET = $$qtLibraryTarget(fugio-lua)
TEMPLATE = lib

CONFIG += plugin c++11

QT += gui widgets

DESTDIR = $$DESTDIR/plugins

DEFINES += LUA_LIBRARY

SOURCES += luaplugin.cpp \
	luanode.cpp \
	syntaxhighlighterlua.cpp \
	luahighlighter.cpp \
	luaexnode.cpp \
	luaexpin.cpp \
	luaarray.cpp

HEADERS += luaplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/lua/uuid.h \
	../../include/fugio/lua/lua_interface.h \
	luanode.h \
	syntaxhighlighterlua.h \
	luahighlighter.h \
	luaexnode.h \
	luaexpin.h \
	luaarray.h

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

	QMAKE_POST_LINK += echo

	exists( /usr/local/opt/lua ) {
		QMAKE_POST_LINK += && mkdir -pv $$DESTDIR/../libs

		QMAKE_POST_LINK += && cp -a /usr/local/opt/lua/lib/*.dylib $$DESTDIR/../libs/

	} else:exists( $$(LIBS)/lua-5.3.3/src ) {
		QMAKE_POST_LINK += && mkdir -pv $$DESTDIR/../libs

		QMAKE_POST_LINK += && cp $$(LIBS)/lua-5.3.3/src/liblua5.3.3.dylib $$DESTDIR/../libs/
	}

	DESTDIR = $$BUNDLEDIR/Contents/MacOS
	DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

	CONFIG(release,debug|release) {
		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		# we don't want to copy the Lua library into the bundle, so change its name

#		QMAKE_POST_LINK += && install_name_tool -change /usr/local/opt/lua/lib/liblua.5.2.dylib liblua.5.2.dylib $$LIBCHANGEDEST

#		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

#		# now change it back

#		QMAKE_POST_LINK += && install_name_tool -change liblua.5.2.dylib /usr/local/lib/liblua.5.2.dylib $$LIBCHANGEDEST

		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs_shared.sh $$BUNDLEDIR/Contents/MacOS

		QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
		QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
		QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

		exists( /usr/local/opt/lua ) {
			QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data/libs

			QMAKE_POST_LINK += && cp -a /usr/local/opt/lua/lib/*.dylib $$INSTALLDIR/data/libs/

		} else:exists( $$(LIBS)/lua-5.3.3/src ) {
			QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data/libs

			QMAKE_POST_LINK += && cp $$(LIBS)/lua-5.3.3/src/liblua5.3.3.dylib $$INSTALLDIR/data/libs/
		}

		QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

		QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
	}
}

windows {
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )

		win32 {
			 QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/Lua-5.3.3/lua53.dll ) $$shell_path( $$INSTALLDIR/data )
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
# Lua

win32:exists( $$(LIBS)/Lua-5.3.3 ) {
	INCLUDEPATH += $$(LIBS)/Lua-5.3.3/include

	LIBS += -L$$(LIBS)/Lua-5.3.3 -llua53

	DEFINES += LUA_SUPPORTED
}

macx {
	exists( /usr/local/opt/lua ) {
		INCLUDEPATH += /usr/local/opt/lua/include

		LIBS += -L/usr/local/opt/lua/lib -llua

		DEFINES += LUA_SUPPORTED

	} else:exists( $$(LIBS)/lua-5.3.3/src ) {
		INCLUDEPATH += $$(LIBS)/lua-5.3.3/src

		LIBS += -L$$(LIBS)/lua-5.3.3/src -llua5.3.3

		DEFINES += LUA_SUPPORTED
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

