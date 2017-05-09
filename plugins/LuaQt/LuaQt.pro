#-------------------------------------------------
#
# Project created by QtCreator 2016-02-25T09:19:59
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

TARGET = $$qtLibraryTarget(fugio-luaqt)
TEMPLATE = lib

CONFIG += plugin c++11

QT += gui widgets

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	luapainter.cpp \
	luapen.cpp \
	luacolor.cpp \
	luabrush.cpp \
	luarectf.cpp \
	luapointf.cpp \
	luasizef.cpp \
	luafont.cpp \
	luafontmetrics.cpp \
	luagradient.cpp \
	luaqtplugin.cpp \
	luaimage.cpp \
	luatransform.cpp \
	luamatrix4x4.cpp \
	luajsondocument.cpp \
	luajsonarray.cpp \
	luajsonobject.cpp \
	luavector3.cpp \
	luaquaternion.cpp

HEADERS +=\
	../../include/fugio/luaqt/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	luapainter.h \
	luapen.h \
	luacolor.h \
	luabrush.h \
	luarectf.h \
	luapointf.h \
	luasizef.h \
	luafont.h \
	luafontmetrics.h \
	luagradient.h \
	luaqtplugin.h \
	luaimage.h \
	luatransform.h \
	luamatrix4x4.h \
	luajsondocument.h \
	luajsonarray.h \
	luajsonobject.h \
	luavector3.h \
	luaquaternion.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_luaqt_de.ts \
	translations/fugio_luaqt_es.ts \
	translations/fugio_luaqt_fr.ts

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

		libraries.path  = $$INSTALLDATA
		libraries.files = $$LUA_PATH/lua53.dll

		INSTALLS += libraries
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

