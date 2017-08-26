#-------------------------------------------------
#
# Project created by QtCreator 2017-02-19T23:00:51
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-dlib)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += dlibplugin.cpp \
	facefeaturesnode.cpp

HEADERS += dlibplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/dlib/uuid.h \
	facefeaturesnode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_dlib_de.ts \
	translations/fugio_dlib_es.ts \
	translations/fugio_dlib_fr.ts

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

	DESTDIR = $$BUNDLEDIR/Contents/MacOS
	DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

	CONFIG(debug,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB
	}

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		isEmpty( CASKBASE ) {
			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins
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
    target.path = $$INSTALLBASE/usr/lib/fugio

    INSTALLS += target
}

#------------------------------------------------------------------------------
# dlib

linux:!macx:exists( /usr/include/dlib ) {
	INCLUDEPATH += /usr/include

	LIBS += -L/usr/lib -ldlib

	DEFINES += DLIB_SUPPORTED
}

macx {
	isEmpty( CASKBASE ) {
		exists( $$(LIBS)/dlib-x64 ) {
			INCLUDEPATH += $$(LIBS)/dlib-x64/include

			DEFINES += DLIB_NO_GUI_SUPPORT

			SOURCES += $$(LIBS)/dlib/dlib/all/source.cpp

			LIBS += -framework Accelerate

#			LIBS += -L/usr/local/lib -ldlib -lblas -llapack

			DEFINES += DLIB_SUPPORTED
		}
	} else {
		exists( /usr/local/include/dlib ) {
			INCLUDEPATH += /usr/local/include

			LIBS += -L/usr/local/lib -ldlib -lblas -llapack

			DEFINES += DLIB_SUPPORTED
		}
	}
}

windows {
	DLIB_INCLUDE_PATH = $$(LIBS)/dlib-19.2

	contains( QT_ARCH, x86_64 ) {
	} else {
		DLIB_PATH = $$(LIBS)/dlib-19.2.32.2015
	}

	exists( $$DLIB_PATH ) {
		INCLUDEPATH += $$DLIB_INCLUDE_PATH

		CONFIG(debug,debug|release) {
			LIBS += -L$$DLIB_PATH/dlib/Debug
		} else {
			LIBS += -L$$DLIB_PATH/dlib/Release
		}

		LIBS += -ldlib

		DEFINES += DLIB_SUPPORTED
	}
}

!contains( DEFINES, DLIB_SUPPORTED ) {
	warning( "DLIB not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
