#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T13:23:33
#
#-------------------------------------------------

QT += gui widgets

TARGET = $$qtLibraryTarget(fugio-fftw)
TEMPLATE = lib
CONFIG += plugin c++11

CONFIG(debug,debug|release) {
    DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
    DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

DEFINES += FFTW_LIBRARY

SOURCES += fftwplugin.cpp \
    fftnode.cpp

HEADERS += fftwplugin.h\
    ../../include/fugio/nodecontrolbase.h \
    ../../include/fugio/fftw/uuid.h \
    fftnode.h

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

        QMAKE_POST_LINK += $$libChange( libfftw3f.3.dylib )

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
    INSTALLDEST  = $$INSTALLDIR/data/plugins/fftw

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += echo

        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

        win32 {
			 QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/fftw-3.3.4/libfftw3f-3.dll ) $$shell_path( $$INSTALLDEST )
        }

        win64 {
			 QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/fftw-3.3.4/libfftw3f-3.dll ) $$shell_path( $$INSTALLDEST )
        }
    }
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------

win32 {
    contains(QMAKE_CC, cl) {
        exists( $$(LIBS)/fftw-3.3.4 ) {
            INCLUDEPATH += $$(LIBS)/fftw-3.3.4

            LIBS += -L$$(LIBS)/fftw-3.3.4 -llibfftw3f-3

            DEFINES += FFTW_PLUGIN_SUPPORTED
        }
    }
}

macx {
    INCLUDEPATH += /opt/local/include

    LIBS += -L/opt/local/lib -lfftw3f

    DEFINES += FFTW_PLUGIN_SUPPORTED
}

unix:!macx:exists( /usr/include ) {
    INCLUDEPATH += /usr/include

    LIBS += -lfftw3f

    DEFINES += FFTW_PLUGIN_SUPPORTED
}

!contains( DEFINES, FFTW_PLUGIN_SUPPORTED ) {
   warning( "FFTW not supported" )
}

