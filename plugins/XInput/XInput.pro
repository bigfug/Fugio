#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27T22:48:02
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-xinput)
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$DESTDIR/plugins

SOURCES += xinputplugin.cpp \
	xinputnode.cpp

HEADERS += xinputplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/xinput/uuid.h \
	xinputnode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_xinput_de.ts \
	translations/fugio_xinput_es.ts \
	translations/fugio_xinput_fr.ts

windows:contains( QMAKE_CC, cl ) {
	LIBS += -lXinput9_1_0
	DEFINES += XINPUT_SUPPORTED
}

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

!contains( DEFINES, XINPUT_SUPPORTED ) {
	warning( "XInput not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

