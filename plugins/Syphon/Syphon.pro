#-------------------------------------------------
#
# Project created by QtCreator 2016-02-18T15:10:39
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets gui

TARGET = $$qtLibraryTarget(fugio-syphon)
TEMPLATE = lib

CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += SYPHON_LIBRARY

SOURCES += \
	syphonpin.cpp \
	syphonreceivernode.cpp \
	syphonsendernode.cpp \
	syphonplugin.cpp \
	syphonrendernode.cpp

OBJECTIVE_SOURCES += \
	syphonsender.mm \
	syphonreceiver.mm

OBJECTIVE_HEADERS += \
	syphonsender.h \
	syphonreceiver.h

HEADERS += \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/syphon/uuid.h \
	opengl_includes.h \
	syphonpin.h \
	syphonreceivernode.h \
	syphonsendernode.h \
	syphonplugin.h \
	syphonreceiver.h \
	syphonsender.h \
	syphonrendernode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_syphon_de.ts \
	translations/fugio_syphon_es.ts \
	translations/fugio_syphon_fr.ts

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

	isEmpty( CASKBASE ) {
		libraries.path  = $$DESTDIR/../libs
		libraries.files = $$(LIBS)/glew-2.0.0/lib/libGLEW.2.0.0.dylib

		INSTALLS += libraries
	}

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

		QMAKE_POST_LINK += && mkdir -pv $$FRAMEWORKDIR

		QMAKE_POST_LINK += && rm -rf $$FRAMEWORKDIR/Syphon.framework

		QMAKE_POST_LINK += && cp -R /Library/Frameworks/Syphon.framework $$FRAMEWORKDIR

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
# OpenGL

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	LIBS += -L/opt/vc/lib -lGLESv2 -lEGL
}

windows {
	INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

	contains( QT_ARCH, x86_64 ) {
		GLEW_PATH = $$(LIBS)/glew.64.2015
	} else {
		GLEW_PATH = $$(LIBS)/glew.32.2015
	}

	CONFIG(release,debug|release) {
		GLEW_PATH = $$GLEW_PATH/lib/Release
	} else {
		GLEW_PATH = $$GLEW_PATH/lib/Debug
	}

	exists( $$GLEW_PATH ) {
		LIBS += -L$$GLEW_PATH

		CONFIG(release,debug|release) {
			LIBS += -llibglew32
		} else {
			LIBS += -llibglew32d
		}

		DEFINES += GLEW_STATIC

		LIBS += -lopengl32
	}
}

macx {
	isEmpty( CASKBASE ) {
		INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

		LIBS += -L$$(LIBS)/glew-2.0.0/lib -lGLEW

	} else {
		INCLUDEPATH += /usr/local/include

		LIBS += -L/usr/local/lib -lGLEW
	}
}

#------------------------------------------------------------------------------
# Syphon

macx {
	QMAKE_CFLAGS += -F/Library/Frameworks
	QMAKE_CXXFLAGS += -F/Library/Frameworks

	QMAKE_OBJECTIVE_CFLAGS += -F/Library/Frameworks
	QMAKE_OBJECTIVE_CXXFLAGS += -F/Library/Frameworks

	LIBS += -F/Library/Frameworks

	LIBS += -framework Syphon
	LIBS += -framework OpenGL
	LIBS += -framework Cocoa
	LIBS += -framework Quartz

	DEFINES += SYPHON_SUPPORTED
}

!contains( DEFINES, SYPHON_SUPPORTED ) {
	warning( "Syphon not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

