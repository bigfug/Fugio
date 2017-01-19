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
	mediasegment.cpp \
    mediarecorderform.cpp \
    mediarecordernode.cpp \
    mediapreset/mediapresetmanager.cpp \
    mediaprocessornode.cpp

HEADERS += \
	../../include/fugio/ffmpeg/uuid.h \
	../../include/fugio/nodecontrolbase.h \
	mediaaudioprocessor.h \
	audiobuffer.h \
	medianode.h \
	imageconvertnode.h \
	ffmpegplugin.h \
	mediasegment.h \
    segmentinterface.h \
    mediarecorderform.h \
    mediarecordernode.h \
    mediapreset/interfacemediapreset.h \
    mediapreset/mediadvdpal.h \
    mediapreset/mediapresetmanager.h \
    mediapreset/mediapresetmp4.h \
    mediapreset/mediaProxy.h \
    mediapreset/mediaWav44.h \
    mediapreset/mediaYouTube240p.h \
    mediapreset/mediaYouTube240pWide.h \
    mediapreset/mediaYouTube360pWide.h \
    mediapreset/mediaYouTube480pWide.h \
    mediapreset/mediaYouTube720pWide.h \
    mediapreset/mediaYouTube1080pWide.h \
    mediaprocessornode.h \
    processoraudiobuffer.h

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
	win32 {
		FFMPEGDIR    = $$(LIBS)/ffmpeg-3.2-win32
	}

	win64 {
		FFMPEGDIR    = $$(LIBS)/ffmpeg-3.2-win64
	}

	exists( $$FFMPEGDIR ) {
		INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
		INSTALLDEST  = $$INSTALLDIR/data/plugins/ffmpeg

		CONFIG(release,debug|release) {
			QMAKE_POST_LINK += echo

			QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

			QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$FFMPEGDIR/bin/*.dll ) $$shell_path( $$INSTALLDEST )
		}

		DEFINES += FFMPEG_SUPPORTED
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

windows:contains( DEFINES, FFMPEG_SUPPORTED ) {
	LIBS += -L$$FFMPEGDIR/lib
	LIBS += -L$$FFMPEGDIR/bin
	INCLUDEPATH += $$FFMPEGDIR/include
	QMAKE_LFLAGS += /OPT:NOREF
}

macx:exists( /usr/local/opt/ffmpeg ) {
	INCLUDEPATH += /usr/local/opt/ffmpeg/include

	LIBS += -L/usr/local/opt/ffmpeg/lib

	DEFINES += FFMPEG_SUPPORTED
}

contains( DEFINES, FFMPEG_SUPPORTED ) {
	LIBS += -lavcodec -lavdevice -lavformat -lavutil -lswscale

	linux:!exists( /usr/include/libswresample/swresample.h ) {
		DEFINES += TL_USE_LIB_AV
	}

	!contains( DEFINES, TL_USE_LIB_AV ) {
		LIBS += -lswresample -lavfilter
	}
}

#------------------------------------------------------------------------------
# hap

contains( DEFINES, FFMPEG_SUPPORTED ) {
	exists( $$(LIBS)/hap/source ) {
		INCLUDEPATH += $$(LIBS)/hap/source

		SOURCES += $$(LIBS)/hap/source/hap.c
		HEADERS += $$(LIBS)/hap/source/hap.h
	}
}

#------------------------------------------------------------------------------
# snappy

contains( DEFINES, FFMPEG_SUPPORTED ) {
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
}

!contains( DEFINES, FFMPEG_SUPPORTED ) {
	warning( "FFMPEG not supported" )
}

FORMS += \
    mediarecorderform.ui
