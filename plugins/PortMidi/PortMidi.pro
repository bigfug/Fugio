#-------------------------------------------------
#
# Project created by QtCreator 2014-07-26T22:35:57
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-portmidi)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

DESTDIR = $$DESTDIR/plugins

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

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
	DEFINES += TARGET_OS_MAC
	CONFIG -= x86
	CONFIG += lib_bundle

	BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
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

windows {
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
	INSTALLDEST  = $$INSTALLDIR/data/plugins/portmidi

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

		win32 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/portmidi.32.2013/Release/portmidi.dll ) $$shell_path( $$INSTALLDEST )
		}

		win64 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/portmidi.64.2013/Release/portmidi.dll ) $$shell_path( $$INSTALLDEST )
		}
	}
}

#------------------------------------------------------------------------------
# portmidi

windows:exists( $$(LIBS)/portmidi ) {
	INCLUDEPATH += $$(LIBS)/portmidi/pm_common
	INCLUDEPATH += $$(LIBS)/portmidi/porttime
}

win64:exists( $$(LIBS)/portmidi.64.2013 ) {
	LIBS += -L$$(LIBS)/portmidi.64.2013/Release

	DEFINES += PORTMIDI_SUPPORTED
}

win32:exists( $$(LIBS)/portmidi.32.2013 )  {
	LIBS += -L$$(LIBS)/portmidi.32.2013/Release

	DEFINES += PORTMIDI_SUPPORTED
}

unix:!macx:exists( /usr/local/include/portmidi.h ) {
	INCLUDEPATH += /usr/local/include
	LIBS += -L/usr/local/lib
	DEFINES += PORTMIDI_SUPPORTED
}

unix:!macx {
	exists( $$[QT_SYSROOT]/usr/include/portmidi.h ) {
		INCLUDEPATH += $$[QT_SYSROOT]/usr/include
		LIBS += -L$$[QT_SYSROOT]/usr/lib
		DEFINES += PORTMIDI_SUPPORTED

	} else:exists( /usr/include/portmidi.h ) {
		INCLUDEPATH += /usr/include
		LIBS += -L/usr/lib
		DEFINES += PORTMIDI_SUPPORTED
	}
}

contains( DEFINES, PORTMIDI_SUPPORTED ) {
	LIBS += -lportmidi
} else {
	message( "PortMidi not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

