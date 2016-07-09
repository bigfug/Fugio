#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T23:09:17
#
#-------------------------------------------------

QT += widgets

TARGET = $$qtLibraryTarget(fugio-image)
TEMPLATE = lib
CONFIG += plugin c++11

CONFIG(debug,debug|release) {
    DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
    DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

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

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
    DEFINES += TARGET_OS_MAC
    CONFIG -= x86
    CONFIG += lib_bundle

    BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
    INSTALLBASE  = $$OUT_PWD/../../../deploy-installer-$$QMAKE_HOST.arch
    INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
    INSTALLDEST  = $$INSTALLDIR/data/plugins
    INCLUDEDEST  = $$INSTALLDIR/data/include/fugio

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

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
        QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

        QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

        QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
    }
}

#------------------------------------------------------------------------------
# Windows

windows {
    INSTALLBASE  = $$OUT_PWD/../../../deploy-installer-$$QMAKE_HOST.arch
    INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
    INSTALLDEST  = $$INSTALLDIR/data/plugins

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += echo

        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/imageformats )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(QTDIR)/plugins/imageformats/"*.dll" ) $$shell_path( $$INSTALLDIR/data/imageformats )

		QMAKE_POST_LINK += & del $$shell_path( $$INSTALLDIR/data/imageformats/"*d.dll" )
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

