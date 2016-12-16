#-------------------------------------------------
#
# Project created by QtCreator 2013-04-21T15:38:11
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += widgets concurrent

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	QT -= opengl
}

TARGET = $$qtLibraryTarget(fugio-opengl)
TEMPLATE = lib
CONFIG += plugin c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += OPENGL_LIBRARY

SOURCES += openglplugin.cpp \
	texturenode.cpp \
	texturepin.cpp \
	syntaxhighlighterglsl.cpp \
	texturecopynode.cpp \
	texturenodeform.cpp \
	shaderinstancenode.cpp \
	shaderpin.cpp \
	viewportnode.cpp \
	bindtexturenode.cpp \
	texturecubenode.cpp \
	clearnode.cpp \
	rendernode.cpp \
	imagetotexturenode.cpp \
	texturetoimagenode.cpp \
	transformfeedbacknode.cpp \
	buffertoarraynode.cpp \
	bufferentrynode.cpp \
	bufferentrypin.cpp \
	bufferentryproxypin.cpp \
	buffernode.cpp \
	contextnode.cpp \
	arraytobuffernode.cpp \
	vertexarrayobjectnode.cpp \
	vertexarrayobjectpin.cpp \
	arraytoindexnode.cpp \
	instancebuffernode.cpp \
	bufferpin.cpp \
	drawnode.cpp \
	preview.cpp \
	previewnode.cpp \
	stateform.cpp \
	statenode.cpp \
	statepin.cpp \
	windownode.cpp \
	shadercompilernode.cpp \
	viewportmatrixnode.cpp \
	renderpin.cpp \
    cubemaprendernode.cpp

HEADERS +=\
	texturenode.h \
	texturepin.h \
	openglplugin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	syntaxhighlighterglsl.h \
	texturecopynode.h \
	texturenodeform.h \
	shaderinstancenode.h \
	shaderpin.h \
	viewportnode.h \
	bindtexturenode.h \
	texturecubenode.h \
	clearnode.h \
	rendernode.h \
	opengl_includes.h \
	imagetotexturenode.h \
	texturetoimagenode.h \
	transformfeedbacknode.h \
	buffertoarraynode.h \
	bufferentrynode.h \
	bufferentrypin.h \
	bufferentryproxypin.h \
	buffernode.h \
	contextnode.h \
	arraytobuffernode.h \
	vertexarrayobjectnode.h \
	vertexarrayobjectpin.h \
	arraytoindexnode.h \
	instancebuffernode.h \
	bufferpin.h \
	drawnode.h \
	preview.h \
	previewnode.h \
	stateform.h \
	statenode.h \
	statepin.h \
	windownode.h \
	../../include/fugio/opengl/uuid.h \
	../../include/fugio/opengl/node_render_interface.h \
	../../include/fugio/opengl/vertex_array_object_interface.h \
	../../include/fugio/opengl/buffer_entry_interface.h \
	../../include/fugio/opengl/buffer_interface.h \
	../../include/fugio/opengl/output_interface.h \
	../../include/fugio/opengl/shader_interface.h \
	../../include/fugio/opengl/texture_interface.h \
	../../include/fugio/opengl/state_interface.h \
	../../include/fugio/render_interface.h \
	shadercompilernode.h \
	viewportmatrixnode.h \
	renderpin.h \
	../../include/fugio/output_interface.h \
    cubemaprendernode.h

FORMS += \
	texturenodeform.ui \
	openglstateform.ui

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	SOURCES += deviceopengloutputrpi.cpp
	HEADERS += deviceopengloutputrpi.h
} else {
	SOURCES += deviceopengloutput.cpp
	HEADERS += deviceopengloutput.h
}

windows {
	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

	LIBS += -lopengl32 -lglu32
}

#------------------------------------------------------------------------------
# Raspberry Pi

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	INCLUDEPATH += $$[QT_SYSROOT]/opt/vc/include $$[QT_SYSROOT]/opt/vc/include/interface/vcos/pthreads $$[QT_SYSROOT]/opt/vc/include/interface/vmcs_host/linux

	LIBS += -L$$[QT_SYSROOT]/opt/vc/lib -lbcm_host -lGLESv2 -lEGL
}

#------------------------------------------------------------------------------
# GLEW

win32 {
	INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

	LIBS += -L$$(LIBS)/glew-2.0.0/lib/Release/Win32 -lglew32s

	DEFINES += GLEW_STATIC

	LIBS += -lopengl32
}

macx {
	QMAKE_POST_LINK += echo

	exists( /usr/local/opt/glew ) {
		QMAKE_POST_LINK += && mkdir -pv $$DESTDIR/../libs

		QMAKE_POST_LINK += && cp -a /usr/local/opt/glew/lib/*.dylib $$DESTDIR/../libs/

		INCLUDEPATH += /usr/local/include

		LIBS += -L/usr/local/lib -lGLEW
	} else:exists( $$(LIBS)/glew-2.0.0 ) {
		INCLUDEPATH += $$(LIBS)/glew-2.0.0/include

		LIBS += $$(LIBS)/glew-2.0.0/lib/libGLEW.a

		DEFINES += GLEW_STATIC
	}
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
		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )
		QMAKE_POST_LINK += $$qtLibChange( QtConcurrent )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs_shared.sh $$BUNDLEDIR/Contents/MacOS

		QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
		QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
		QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

		QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

		QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST

		exists( /usr/local/opt/glew ) {
			QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data/libs

			QMAKE_POST_LINK += && cp -a /usr/local/opt/glew/lib/*.dylib $$INSTALLDIR/data/libs/
		}
	}
}

#------------------------------------------------------------------------------
# Windows

windows {
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio
	INSTALLDEST  = $$INSTALLDIR/data/plugins

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDEST )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDEST )

		QMAKE_POST_LINK += & for %I in ( $$shell_path( $(QTDIR)/bin/Qt5OpenGL.dll ) ) do copy %I $$shell_path( $$INSTALLDIR/data/ )
	}
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
	INSTALLDIR = $$INSTALLBASE/packages/com.bigfug.fugio

	contains( DEFINES, Q_OS_RASPBERRY_PI ) {
		target.path = Desktop/Fugio/plugins
	} else {
		target.path = $$shell_path( $$INSTALLDIR/data/plugins )
	}

	INSTALLS += target
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$FUGIO_ROOT/Fugio/include

