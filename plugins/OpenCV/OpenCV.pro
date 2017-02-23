#-------------------------------------------------
#
# Project created by QtCreator 2014-07-19T19:23:49
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-opencv)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	inpaintnode.cpp \
	imagethresholdnode.cpp \
	grayscalenode.cpp \
	videocapturenode.cpp \
	imageconvertnode.cpp \
	imagehomographynode.cpp \
	devicevideocapture.cpp \
	opencvplugin.cpp \
	backgroundsubtractionnode.cpp \
	inrangenode.cpp \
	erodenode.cpp \
	dilatenode.cpp \
	findcontoursnode.cpp \
	resizenode.cpp \
	equalizehistnode.cpp \
	cascadeclassifiernode.cpp \
    momentsnode.cpp

HEADERS +=\
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/opencv/uuid.h \
	inpaintnode.h \
	imagethresholdnode.h \
	grayscalenode.h \
	videocapturenode.h \
	imageconvertnode.h \
	imagehomographynode.h \
	devicevideocapture.h \
	opencvplugin.h \
	backgroundsubtractionnode.h \
	inrangenode.h \
	erodenode.h \
	dilatenode.h \
	findcontoursnode.h \
	resizenode.h \
	equalizehistnode.h \
	cascadeclassifiernode.h \
    momentsnode.h

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

		QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins

		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$FRAMEWORKDIR
		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS

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
	OPENCV_DIR = $$(LIBS)/opencv-3.2.0
	OPENCV_VER = 320

	exists( $$OPENCV_DIR ) {
		INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
		INSTALLDEST  = $$INSTALLDIR/data/plugins/opencv

		CONFIG(release,debug|release) {
			QMAKE_POST_LINK += echo

			QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$OPENCV_DIR/build/bin/Release/*.dll ) $$shell_path( $$INSTALLDEST )
		}
	}
}

#------------------------------------------------------------------------------

macx:exists( /usr/local/opt/opencv3 ) {
	INCLUDEPATH += /usr/local/opt/opencv3/include

	LIBS += -L/usr/local/opt/opencv3/lib -lopencv_core -lopencv_imgproc -lopencv_photo -lopencv_highgui -lopencv_video -lopencv_videoio -lopencv_videoio -lopencv_objdetect

	DEFINES += OPENCV_SUPPORTED
}

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

win32:exists( $$OPENCV_DIR/build/lib ) {
	INCLUDEPATH += $$OPENCV_DIR/build/include

	CONFIG(debug,debug|release) {
		LIBS += -L$$OPENCV_DIR/build/lib/Debug -lopencv_core$${OPENCV_VER}d -lopencv_imgproc$${OPENCV_VER}d -lopencv_photo$${OPENCV_VER}d -lopencv_highgui$${OPENCV_VER}d -lopencv_video$${OPENCV_VER}d -lopencv_videoio$${OPENCV_VER}d -lopencv_objdetect$${OPENCV_VER}d
	} else {
		LIBS += -L$$OPENCV_DIR/build/lib/Release -lopencv_core$${OPENCV_VER} -lopencv_imgproc$${OPENCV_VER} -lopencv_photo$${OPENCV_VER} -lopencv_highgui$${OPENCV_VER} -lopencv_video$${OPENCV_VER} -lopencv_videoio$${OPENCV_VER} -lopencv_objdetect$${OPENCV_VER}
	}

	DEFINES += OPENCV_SUPPORTED
}

!contains( DEFINES, OPENCV_SUPPORTED ) {
	warning( "OPENCV not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
