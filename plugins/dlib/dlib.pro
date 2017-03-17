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

			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$FRAMEWORKDIR
			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

windows {
	INSTALLDIR = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )
	}
}

#------------------------------------------------------------------------------
# dlib

linux:!macx:exists( /usr/include/dlib ) {
	INCLUDEPATH += /usr/include

	LIBS += -L/usr/lib -ldlib

	DEFINES += DLIB_SUPPORTED
}

macx:exists( /usr/local/include/dlib ) {
	INCLUDEPATH += /usr/local/include

	LIBS += -L/usr/local/lib -ldlib -lblas -llapack

	DEFINES += DLIB_SUPPORTED
}

win32:exists( $$(LIBS)/dlib-19.2 ) {
	INCLUDEPATH += $$(LIBS)/dlib-19.2

	CONFIG(debug,debug|release) {
		LIBS += -L$$(LIBS)/dlib-19.2.32.2015/dlib/Debug
	} else {
		LIBS += -L$$(LIBS)/dlib-19.2.32.2015/dlib/Release
	}

	LIBS += -ldlib

	DEFINES += DLIB_SUPPORTED
}

!contains( DEFINES, DLIB_SUPPORTED ) {
	warning( "DLIB not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
