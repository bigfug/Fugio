#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T18:48:46
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += gui widgets

TARGET = $$qtLibraryTarget(fugio-isf)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	isfplugin.cpp \
    isfnode.cpp \
    settingsform.cpp

HEADERS += \
	../../include/fugio/isf/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	isfplugin.h \
    isfnode.h \
    opengl_includes.h \
    settingsform.h

RESOURCES += \
	resources.qrc

FORMS += \
    settingsform.ui

TRANSLATIONS = \
	translations/fugio_isf_de.ts \
	translations/fugio_isf_es.ts \
	translations/fugio_isf_fr.ts

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

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

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

	QMAKE_CFLAGS += /bigobj
	QMAKE_CXXFLAGS += /bigobj
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
    target.path = $$INSTALLBASE/usr/lib/fugio

    INSTALLS += target
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------
# GLEW

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

		DEFINES += GLEW_SUPPORTED
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

	DEFINES += GLEW_SUPPORTED
}
