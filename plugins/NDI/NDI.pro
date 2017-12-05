#-------------------------------------------------
#
# Project created by QtCreator 2016-07-23T20:30:54
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-ndi)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += ndiplugin.cpp \
	ndireceivenode.cpp \
	ndisendnode.cpp

HEADERS += ndiplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/ndi/uuid.h \
	ndireceivenode.h \
	ndisendnode.h

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

	CONFIG(debug,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += && install_name_tool -change @rpath/libndi.dylib $$(LIBS)/NDI/lib/x64/libndi.dylib $$LIBCHANGEDEST
	}

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		QMAKE_POST_LINK += && install_name_tool -change @rpath/libndi.dylib $$(LIBS)/NDI/bin/x64/libndi.dylib $$DESTLIB

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$FRAMEWORKDIR
		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS

		contains( DEFINES, INTERNAL_BUILD ) {
			plugin.path = $$INSTALLDEST
			plugin.files = $$BUNDLEDIR
			plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

			INSTALLS += plugin
		}
	}
}

#------------------------------------------------------------------------------
# Windows

windows {
	INSTALLDEST  = $$INSTALLDATA/plugins/ndi

	plugin.path  = $$INSTALLDEST
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin

	NDI_PATH = "C:/Program Files/NewTek/NewTek NDI Redist/v2"

	contains( QT_ARCH, x86_64 ) {
		NDI_LIB = x64
	} else {
		NDI_LIB = x86
	}

	exists( $$NDI_PATH ) {
		libraries.path  = $$INSTALLDEST
		libraries.files = $$NDI_PATH/Processing.NDI.Lib.$${NDI_LIB}.dll

		INSTALLS += libraries
	}
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
	target.path = $$INSTALLBASE/usr/lib/fugio

	INSTALLS += target
}

#------------------------------------------------------------------------------
# NDI

macx:exists( $$(LIBS)/NDI ) {
	INCLUDEPATH += $$(LIBS)/NDI/include

	LIBS += -L$$(LIBS)/NDI/lib/x64 -lndi

#	DEFINES += NDI_SUPPORTED
}

windows:exists( "C:/Program Files/NewTek/NewTek NDI SDK" ) {
	INCLUDEPATH += "C:/Program Files/NewTek/NewTek NDI SDK/Include"

	contains( QT_ARCH, x86_64 ) {
		LIBS += -L"C:/Program Files/NewTek/NewTek NDI SDK/Lib/x64" -lProcessing.NDI.Lib.x64
	} else {
		LIBS += -L"C:/Program Files/NewTek/NewTek NDI SDK/Lib/x86" -lProcessing.NDI.Lib.x86
	}

	DEFINES += NDI_SUPPORTED
}

!contains( DEFINES, NDI_SUPPORTED ) {
	warning( "NDI not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
