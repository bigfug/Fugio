#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T13:14:29
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT       += widgets

TARGET = $$qtLibraryTarget(fugio-ffmpeg)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

SOURCES += \
	mediaaudioprocessor.cpp \
	audiobuffer.cpp \
	medianode.cpp \
	imageconvertnode.cpp \
	ffmpegplugin.cpp \
	mediasegment.cpp

HEADERS += \
	../../include/fugio/ffmpeg/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	mediaaudioprocessor.h \
	audiobuffer.h \
	medianode.h \
	imageconvertnode.h \
	ffmpegplugin.h \
	mediasegment.h \
    segmentinterface.h

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
}

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
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
	INSTALLDEST  = $$INSTALLDIR/data/plugins/ffmpeg

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

		win32 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/ffmpeg-3.0.1-32/bin/*.dll ) $$shell_path( $$INSTALLDEST )
		}

		win64 {
			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$(LIBS)/ffmpeg-3.0.1-64/bin/*.dll ) $$shell_path( $$INSTALLDEST )
		}
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

#------------------------------------------------------------------------------
# ffmpeg

linux {
	LIBS += -L/usr/local/lib
}

win32 {
	LIBS += -L$$(LIBS)/ffmpeg-20160728-fb91143-win32/lib
	LIBS += -L$$(LIBS)/ffmpeg-20160728-fb91143-win32/bin
	INCLUDEPATH += $$(LIBS)/ffmpeg-20160728-fb91143-win32/include
	QMAKE_LFLAGS += /OPT:NOREF
}

win64 {
	LIBS += -L$$(LIBS)/ffmpeg-2.8.3-64/lib
	LIBS += -L$$(LIBS)/ffmpeg-2.8.3-64/bin
	INCLUDEPATH += $$(LIBS)/ffmpeg-2.8.3-64/include
	QMAKE_LFLAGS += /OPT:NOREF
}

macx {
	INCLUDEPATH += /usr/local/opt/ffmpeg/include
	LIBS += -L/usr/local/opt/ffmpeg/lib
}

LIBS += -lavcodec -lavdevice -lavformat -lavutil -lswscale

linux:!exists( /usr/include/libswresample/swresample.h ) {
	DEFINES += TL_USE_LIB_AV
}

!contains( DEFINES, TL_USE_LIB_AV ) {
	LIBS += -lswresample -lavfilter
}

#------------------------------------------------------------------------------
# hap

exists( $$(LIBS)/hap/source ) {
	INCLUDEPATH += $$(LIBS)/hap/source

	SOURCES += $$(LIBS)/hap/source/hap.c
	HEADERS += $$(LIBS)/hap/source/hap.h
}

#------------------------------------------------------------------------------
# snappy

macx {
	INCLUDEPATH += /usr/local/opt/snappy/include
	LIBS += -L/usr/local/opt/snappy/lib
}

unix {
#    INCLUDEPATH += $$(LIBS)/snappy

#    SOURCES += $$(LIBS)/snappy/snappy-c.cc \
#        $$(LIBS)/snappy/snappy.cc
#    HEADERS += $$(LIBS)/snappy/snappy.h \
#        $$(LIBS)/snappy/snappy-c.h

	LIBS += -lsnappy
}

DEFINES += SNAPPY_STATIC

windows {
	INCLUDEPATH += $$(LIBS)/snappy-1.1.1.8

	SOURCES += $$(LIBS)/snappy-1.1.1.8/snappy.cc \
		$$(LIBS)/snappy-1.1.1.8/snappy-c.cc \
		$$(LIBS)/snappy-1.1.1.8/snappy-sinksource.cc \
		$$(LIBS)/snappy-1.1.1.8/snappy-stubs-internal.cc

	HEADERS += $$(LIBS)/snappy-1.1.1.8/snappy.h \
		$$(LIBS)/snappy-1.1.1.8/snappy-c.h \
		$$(LIBS)/snappy-1.1.1.8/snappy-internal.h \
		$$(LIBS)/snappy-1.1.1.8/snappy-sinksource.h \
		$$(LIBS)/snappy-1.1.1.8/snappy-stubs-internal.h \
		$$(LIBS)/snappy-1.1.1.8/snappy-stubs-public.h
}

