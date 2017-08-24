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
	momentsnode.cpp \
	distancetransformnode.cpp \
	converttonode.cpp \
	addnode.cpp \
	flipnode.cpp \
    houghlinesnode.cpp \
    simpleblobdetectornode.cpp

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
	momentsnode.h \
	distancetransformnode.h \
	converttonode.h \
	addnode.h \
	flipnode.h \
    houghlinesnode.h \
    simpleblobdetectornode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_opencv_de.ts \
	translations/fugio_opencv_es.ts \
	translations/fugio_opencv_fr.ts

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

		isEmpty( CASKBASE ) {
			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins -verbose=2

			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$FRAMEWORKDIR
			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs.sh $$BUNDLEDIR/Contents/MacOS
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

#------------------------------------------------------------------------------
# Windows

windows {
	INSTALLDEST  = $$INSTALLDATA/plugins/opencv

	plugin.path  = $$INSTALLDEST
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin
}

#------------------------------------------------------------------------------

macx {
	isEmpty( CASKBASE ) {
		OPENCV_PATH = $$(LIBS)/opencv-3.1.0-x64
	} else {
		OPENCV_PATH = /usr/local/opt/opencv3
	}

	exists( $$OPENCV_PATH ) {
		INCLUDEPATH += $$OPENCV_PATH/include

		LIBS += -L$$OPENCV_PATH/lib

		DEFINES += OPENCV_SUPPORTED
	}

	contains( DEFINES, OPENCV_SUPPORTED ) {
		LIBS += -lopencv_imgproc -lopencv_photo -lopencv_video -lopencv_objdetect

		exists( $$OPENCV_PATH/lib/libopencv_core.* ) {
			LIBS += -lopencv_core
		}

		exists( $$OPENCV_PATH/lib/libopencv_videoio.* ) {
			LIBS += -lopencv_videoio
		}

		exists( $$OPENCV_PATH/lib/libopencv_highgui.* ) {
			LIBS += -lopencv_highgui
		}

		exists( $$OPENCV_PATH/lib/libopencv_features2d.* ) {
			LIBS += -lopencv_features2d
		}
	}
}

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

windows:contains( QT_ARCH, i386 ) {
	OPENCV_DIR = $$(LIBS)/opencv-3.2.0
	OPENCV_VER = 320

	exists( $$OPENCV_DIR/build/lib ) {
		INCLUDEPATH += $$OPENCV_DIR/build/include

		CONFIG(debug,debug|release) {
			LIBS += -L$$OPENCV_DIR/build/lib/Debug -lopencv_core$${OPENCV_VER}d -lopencv_imgproc$${OPENCV_VER}d  -lopencv_features2d$${OPENCV_VER}d -lopencv_photo$${OPENCV_VER}d -lopencv_highgui$${OPENCV_VER}d -lopencv_video$${OPENCV_VER}d -lopencv_videoio$${OPENCV_VER}d -lopencv_objdetect$${OPENCV_VER}d
		} else {
			LIBS += -L$$OPENCV_DIR/build/lib/Release -lopencv_core$${OPENCV_VER} -lopencv_imgproc$${OPENCV_VER} -lopencv_features2d$${OPENCV_VER} -lopencv_photo$${OPENCV_VER} -lopencv_highgui$${OPENCV_VER} -lopencv_video$${OPENCV_VER} -lopencv_videoio$${OPENCV_VER} -lopencv_objdetect$${OPENCV_VER}
		}

		DEFINES += OPENCV_SUPPORTED

		libraries.path  = $$INSTALLDEST
		libraries.files = $$OPENCV_DIR/build/bin/Release/*.dll

		INSTALLS += libraries
	}
}

linux:!macx {
	exists( $$[QT_SYSROOT]/usr/include/opencv2 ) {
#		INCLUDEPATH += $$[QT_SYSROOT]/usr/include

		LIBS += -lopencv_core -lopencv_imgproc -lopencv_photo -lopencv_highgui -lopencv_video -lopencv_objdetect -lopencv_features2d

		DEFINES += OPENCV_SUPPORTED
	} else:exists( /usr/include/opencv2 ) {
		INCLUDEPATH += /usr/include

		LIBS += -L/usr/lib -lopencv_core -lopencv_imgproc -lopencv_photo -lopencv_highgui -lopencv_video -lopencv_objdetect -lopencv_features2d

		DEFINES += OPENCV_SUPPORTED
	}
}

!contains( DEFINES, OPENCV_SUPPORTED ) {
	warning( "OPENCV not supported" )
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include
