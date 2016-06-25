#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T10:36:16
#
#-------------------------------------------------

QT -= gui
QT += widgets concurrent network

TARGET = $$qtLibraryTarget(fugio)
TEMPLATE = lib
CONFIG += c++11

CONFIG(debug,debug|release) {
	DESTDIR = $$OUT_PWD/../../deploy-debug-$$QMAKE_HOST.arch
} else {
	DESTDIR = $$OUT_PWD/../../deploy-release-$$QMAKE_HOST.arch
}

DEFINES += FUGIOLIB_LIBRARY

SOURCES += fugio.cpp \
    contextprivate.cpp \
    context.cpp \
    globalprivate.cpp \
    global.cpp \
    pinprivate.cpp \
    pin.cpp \
    nodeprivate.cpp \
    node.cpp \
    interpolation.cpp

HEADERS += fugio.h\
    ../include/fugio/global.h \
    contextprivate.h \
    globalprivate.h \
    pinprivate.h \
    nodeprivate.h \
    ../include/fugio/interpolation.h \
    ../include/fugio/performance.h \
    ../stable.h \
    ../include/fugio/pluginbase.h \
    ../include/fugio/choice_interface.h \
    ../include/fugio/global_interface.h \
    ../include/fugio/plugin_interface.h \
    ../include/fugio/serialise_interface.h \
    ../include/fugio/pin_signals.h \
    ../include/fugio/node_signals.h \
    ../include/fugio/playhead_interface.h \
    ../include/fugio/pin_control_interface.h \
    ../include/fugio/pin_interface.h \
    ../include/fugio/node_control_interface.h \
    ../include/fugio/node_interface.h \
    ../include/fugio/edit_interface.h \
    ../include/fugio/device_factory_interface.h \
    ../include/fugio/context_widget_interface.h \
    ../include/fugio/context_interface.h \
    ../include/fugio/global_signals.h \
    ../include/fugio/utils.h \
    ../include/fugio/context_signals.h \
    ../include/fugio/paired_pins_helper_interface.h \
    ../include/fugio/menu_control_interface.h

TRANSLATIONS = fugio_fr.ts

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
    DEFINES += TARGET_OS_MAC

	INSTALLDIR = $$OUT_PWD/../../deploy-installer-$$QMAKE_HOST.arch/packages/com.bigfug.fugio

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += mkdir -pv $$INSTALLDIR/data
        QMAKE_POST_LINK += && cp -R $$DESTDIR/libfugio* $$INSTALLDIR/data
    }
}

#------------------------------------------------------------------------------

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

	INSTALLDIR = $$OUT_PWD/../../deploy-installer-$$QMAKE_HOST.arch/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/ )
	}
}

#------------------------------------------------------------------------------
# Precompiled Headers

CONFIG += precompile_header

PRECOMPILED_HEADER = ../stable.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../include

#------------------------------------------------------------------------------
# Raspberry Pi

linux:!mac:exists( /opt/vc/include/bcm_host.h ) {
    DEFINES += Q_OS_RASPERRY_PI

    LIBS += -L/opt/vc/lib -lGLESv2 -lEGL
}
