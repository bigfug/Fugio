#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T18:48:46
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += gui widgets

TARGET = $$qtLibraryTarget(fugio-isf)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	isfplugin.cpp

HEADERS += \
	../../include/fugio/isf/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	isfplugin.h

RESOURCES += \
	resources.qrc \
    resources.qrc

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
