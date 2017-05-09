#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T13:23:33
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += gui widgets

TARGET = $$qtLibraryTarget(fugio-fftw)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += FFTW_LIBRARY

SOURCES += fftwplugin.cpp \
	fftnode.cpp

HEADERS += fftwplugin.h\
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/fftw/uuid.h \
	fftnode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_fftw_de.ts \
	translations/fugio_fftw_es.ts \
	translations/fugio_fftw_fr.ts

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

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		isEmpty( CASKBASE ) {
			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/Frameworks
			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

#------------------------------------------------------------------------------
# Windows

windows {
	INSTALLDEST  = $$INSTALLDATA/plugins/fftw

	plugin.path  = $$INSTALLDEST
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
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------

windows {
	contains( QT_ARCH, x86_64 ) {
		FFTW_PATH = $$(LIBS)/fftw-3.3.5-dll64
	} else {
		FFTW_PATH = $$(LIBS)/fftw-3.3.5
	}

	exists( $$FFTW_PATH ) {
		INCLUDEPATH += $$FFTW_PATH

		LIBS += -L$$FFTW_PATH -llibfftw3f-3

		DEFINES += FFTW_PLUGIN_SUPPORTED
	}

	libraries.path  = $$INSTALLDEST
	libraries.files = $$FFTW_PATH/libfftw3f-3.dll

	INSTALLS += libraries
}

macx {
	isEmpty( CASKBASE ) {
		FFTW_PATH = $$(LIBS)/fftw-3.3.4-x64

		exists( $$FFTW_PATH ) {
			INCLUDEPATH += $$FFTW_PATH/include

			LIBS += -L$$FFTW_PATH/lib -lfftw3f

			DEFINES += FFTW_PLUGIN_SUPPORTED
		}
	} else {
		exists( /usr/local/opt/fftw ) {
			INCLUDEPATH += /usr/local/opt/fftw/include

			LIBS += -L/usr/local/opt/fftw/lib -lfftw3f

			DEFINES += FFTW_PLUGIN_SUPPORTED
		}
	}
}

unix:!macx {
	exists( $$[QT_SYSROOT]/usr/include/fftw3.h ) {
		INCLUDEPATH += $$[QT_SYSROOT]/usr/include

		LIBS += -lfftw3f

		DEFINES += FFTW_PLUGIN_SUPPORTED

	} else:exists( /usr/include/fftw3.h ) {
		INCLUDEPATH += /usr/include

		LIBS += -lfftw3f

		DEFINES += FFTW_PLUGIN_SUPPORTED
	}
}

!contains( DEFINES, FFTW_PLUGIN_SUPPORTED ) {
   warning( "FFTW not supported" )
}

