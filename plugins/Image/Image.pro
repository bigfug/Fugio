#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T23:09:17
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += widgets

TARGET = $$qtLibraryTarget(fugio-image)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += IMAGE_LIBRARY

SOURCES += imageplugin.cpp \
	imagepreviewnode.cpp \
	imagepreview.cpp \
	imagepin.cpp \
	grabscreennode.cpp \
	scaleimagenode.cpp \
	imageloadernode.cpp \
	imagenode.cpp \
	painterwindow.cpp \
	painterwindownode.cpp \
	painterpin.cpp \
	colourmasknode.cpp \
	imagesizenode.cpp \
	imagefilternode.cpp \
	imagesavenode.cpp

HEADERS += imageplugin.h\
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	../../include/fugio/image/painter_interface.h \
	imagepin.h \
	imagepreviewnode.h \
	imagepreview.h \
	grabscreennode.h \
	scaleimagenode.h \
	imageloadernode.h \
	imagenode.h \
	painterwindow.h \
	painterwindownode.h \
	painterpin.h \
	colourmasknode.h \
	imagesizenode.h \
	imagefilternode.h \
	imagesavenode.h \
	../../include/fugio/image/uuid.h \
	../../include/fugio/image/image_interface.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_image_de.ts \
	translations/fugio_image_es.ts \
	translations/fugio_image_fr.ts

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

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

#------------------------------------------------------------------------------
# Windows

windows {
	plugin.path  = $$INSTALLDATA/plugins
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin

	imageformats.path  = $$INSTALLDATA/imageformats
	imageformats.files = $$(QTDIR)/plugins/imageformats/*[^d].dll

	INSTALLS += imageformats
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

