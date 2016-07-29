#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T09:23:05
#
#-------------------------------------------------

TARGET = $$qtLibraryTarget(fugio-lua)
TEMPLATE = lib

CONFIG += plugin c++11

QT += gui widgets

CONFIG(debug,debug|release) {
    DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
    DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

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
    INSTALLBASE  = $$OUT_PWD/../../../deploy-installer-$$QMAKE_HOST.arch
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

        # we don't want to copy the Lua library into the bundle, so change its name

        QMAKE_POST_LINK += && install_name_tool -change /usr/local/opt/lua/lib/liblua.5.2.dylib liblua.5.2.dylib $$LIBCHANGEDEST

        QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

        # now change it back

        QMAKE_POST_LINK += && install_name_tool -change liblua.5.2.dylib /usr/local/lib/liblua.5.2.dylib $$LIBCHANGEDEST

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
        QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

        QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

        QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
    }
}

windows {
    INSTALLBASE  = $$OUT_PWD/../../../deploy-installer-$$QMAKE_HOST.arch
    INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += echo

        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )

        win32 {
             QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/Lua-5.3.2/lua53.dll ) $$shell_path( $$INSTALLDIR/data )
        }
    }
}

#------------------------------------------------------------------------------
# Lua

win32:exists( $$(LIBS)/Lua-5.3.2 ) {
    INCLUDEPATH += $$(LIBS)/Lua-5.3.2/include

    LIBS += -L$$(LIBS)/Lua-5.3.2 -llua53

    DEFINES += LUA_SUPPORTED
}

macx:exists( /usr/local/include/lua.hpp ) {
    INCLUDEPATH += /usr/local/include

    LIBS += -L/usr/local/lib -llua

    DEFINES += LUA_SUPPORTED
}

unix:!macx:exists( /usr/include/lua5.3/lua.h ) {
    INCLUDEPATH += /usr/include/lua5.3

    LIBS += -llua5.3

    DEFINES += LUA_SUPPORTED
}

!contains( DEFINES, LUA_SUPPORTED ) {
    warning( "Lua not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

