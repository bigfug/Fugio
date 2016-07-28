#-------------------------------------------------
#
# Project created by QtCreator 2014-07-26T22:35:57
#
#-------------------------------------------------

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-portmidi)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

CONFIG(debug,debug|release) {
    DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
    DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

SOURCES += \
    devicemidi.cpp \
    portmidiplugin.cpp \
    portmidiinputnode.cpp \
    portmidioutputnode.cpp

HEADERS += \
    ../../include/fugio/nodecontrolbase.h \
    ../../include/fugio/pincontrolbase.h \
    ../../include/fugio/portmidi/uuid.h \
    devicemidi.h \
    portmidiplugin.h \
    portmidiinputnode.h \
    portmidioutputnode.h

FORMS +=

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

        # we don't want to copy the Lua library into the bundle, so change its name

        QMAKE_POST_LINK += && install_name_tool -change /usr/local/opt/portmidi/lib/libportmidi.dylib libportmidi.dylib $$LIBCHANGEDEST

        QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

        # now change it back

        QMAKE_POST_LINK += && install_name_tool -change libportmidi.dylib /usr/local/lib/libportmidi.dylib $$LIBCHANGEDEST

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
        QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

        QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

        QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
    }
}

#------------------------------------------------------------------------------
# portmidi

windows {
	LIBS += -L$$(LIBS)/portmidi.32.2013/Release
	INCLUDEPATH += $$(LIBS)/portmidi/pm_common
	INCLUDEPATH += $$(LIBS)/portmidi/porttime
}

win64 {
	LIBS += -L$$(LIBS)/portmidi.64.2013/Release
}

win32 {
	LIBS += -L$$(LIBS)/portmidi.32.2013/Release
}

macx {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib
}

LIBS += -lportmidi

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

