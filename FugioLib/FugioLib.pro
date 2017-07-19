#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T10:36:16
#
#-------------------------------------------------

include( ../FugioGlobal.pri )

QT -= gui
QT += widgets concurrent network

TARGET = $$qtLibraryTarget(fugio)
TEMPLATE = lib
CONFIG += c++11

contains( DEFINES, FUGIOLIB_STATIC ) {
	CONFIG += staticlib
}

DEFINES += FUGIOLIB_LIBRARY
DEFINES += GLOBAL_THREADED

SOURCES += fugio.cpp \
	contextprivate.cpp \
	context.cpp \
	globalprivate.cpp \
	global.cpp \
	pinprivate.cpp \
	pin.cpp \
	nodeprivate.cpp \
	node.cpp \
	interpolation.cpp \
    timesync.cpp

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
    timesync.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_lib_de.ts \
	translations/fugio_lib_es.ts \
	translations/fugio_lib_fr.ts

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
	DEFINES += TARGET_OS_MAC

	!contains( DEFINES, FUGIOLIB_STATIC ) {
		install.path = $$INSTALLDATA
		install.files = $$DESTDIR/libfugio.1.dylib

		INSTALLS += install
	}
}

#------------------------------------------------------------------------------

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

	!contains( DEFINES, FUGIOLIB_STATIC ) {
		install.path = $$INSTALLDATA
		install.files = $$DESTDIR/$$TARGET".dll"

		INSTALLS += install
	}
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
	!contains( DEFINES, FUGIOLIB_STATIC ) {
		contains( DEFINES, Q_OS_RASPBERRY_PI ) {
			target.path = Desktop/Fugio
		} else {
			target.path = $$INSTALLDATA
		}

		INSTALLS += target
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

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	LIBS += -L/opt/vc/lib -lGLESv2 -lEGL
}
