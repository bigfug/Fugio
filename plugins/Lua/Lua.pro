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

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_lua_de.ts \
	translations/fugio_lua_es.ts \
	translations/fugio_lua_fr.ts

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
	DEFINES += TARGET_OS_MAC
	CONFIG -= x86
	CONFIG += lib_bundle

	BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
	INSTALLDEST  = $$INSTALLDATA/plugins
	INCLUDEDEST  = $$INSTALLDATA/include/fugio

	isEmpty( CASKBASE ) {
		libraries.path  = $$DESTDIR/../libs
		libraries.files = $$(LIBS)/lua-x64/lib/liblua5.3.4.dylib

		INSTALLS += libraries

		library_id.path = $$DESTDIR/../libs
		library_id.depends = install_libraries
		library_id.commands = install_name_tool -id @rpath/liblua5.3.4.dylib $$library_id.path/liblua5.3.4.dylib

		INSTALLS += library_id
	}

	QMAKE_POST_LINK += echo

	DESTDIR = $$BUNDLEDIR/Contents/MacOS
	DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

	CONFIG(release,debug|release) {
		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		isEmpty( CASKBASE ) {
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

	libraries.path  = $$INSTALLDATA
	libraries.files = $$(LIBS)/Lua-5.3.3/lua53.dll

	INSTALLS += libraries
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

