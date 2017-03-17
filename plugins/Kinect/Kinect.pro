#-------------------------------------------------
#
# Project created by QtCreator 2013-04-01T11:38:11
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

TARGET = $$qtLibraryTarget(fugio-kinect)
TEMPLATE = lib
CONFIG += plugin
QT += gui widgets

DESTDIR = $$DESTDIR/plugins

DEFINES += KINECT_LIBRARY

SOURCES += \
	kinectnode.cpp \
	kinectplugin.cpp \
	kinectconfigdialog.cpp

HEADERS +=\
	kinectnode.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/kinect/uuid.h \
	kinectplugin.h \
	kinectconfigdialog.h \
	interactionclient.h

FORMS += \
	kinectconfigdialog.ui

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

#------------------------------------------------------------------------------
# Windows plugin bundle

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------

win32:exists( "C:/Program Files/Microsoft SDKs/Kinect/v1.8" ) {
	INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Kinect/v1.8/inc"

	LIBS += -L"C:/Program Files/Microsoft SDKs/Kinect/v1.8/lib/x86"
	LIBS += -lKinect10
	DEFINES += KINECT_SUPPORTED
}

#win32:exists( "C:/Program Files/Microsoft SDKs/Kinect/Developer Toolkit v1.8.0" ) {
#	INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Kinect/Developer Toolkit v1.8.0/inc"

#	LIBS += -L"C:/Program Files/Microsoft SDKs/Kinect/Developer Toolkit v1.8.0/lib/x86"

#	LIBS += -lKinectInteraction180_32
#}

!contains( DEFINES, KINECT_SUPPORTED ) {
	warning( "Microsoft Kinect not supported" )
}
