#-------------------------------------------------
#
# Project created by QtCreator 2013-04-21T15:38:11
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += widgets concurrent opengl

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
	cubemaprendernode.cpp \
	textureclonenode.cpp \
    texturemonitornode.cpp \
    texturemonitor.cpp \
    rendertotexturenode.cpp

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
	cubemaprendernode.h \
	textureclonenode.h \
    texturemonitornode.h \
    texturemonitor.h \
    rendertotexturenode.h

FORMS += \
	texturenodeform.ui \
	openglstateform.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_opengl_de.ts \
	translations/fugio_opengl_es.ts \
	translations/fugio_opengl_fr.ts

#contains( DEFINES, Q_OS_RASPBERRY_PI ) {
#	SOURCES += deviceopengloutputrpi.cpp
#	HEADERS += deviceopengloutputrpi.h
#} else {
	SOURCES += deviceopengloutput.cpp
	HEADERS += deviceopengloutput.h
#}

#windows {
#	QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

#	LIBS += -lopengl32 -lglu32
#}

#------------------------------------------------------------------------------
# Raspberry Pi

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	INCLUDEPATH += $$[QT_SYSROOT]/opt/vc/include $$[QT_SYSROOT]/opt/vc/include/interface/vcos/pthreads $$[QT_SYSROOT]/opt/vc/include/interface/vmcs_host/linux

	LIBS += -L$$[QT_SYSROOT]/opt/vc/lib -lbcm_host -lGLESv2 -lEGL
}

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

	isEmpty( CASKBASE ) {
		libraries.path  = $$DESTDIR/../libs
		libraries.files = $$(LIBS)/glew-2.0.0/lib/libGLEW.2.0.0.dylib

		INSTALLS += libraries
	}

	DESTDIR = $$BUNDLEDIR/Contents/MacOS
	DESTLIB = $$DESTDIR/"lib"$$TARGET".dylib"

	CONFIG(release,debug|release) {
		LIBCHANGEDEST = $$DESTLIB

		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += $$qtLibChange( QtWidgets )
		QMAKE_POST_LINK += $$qtLibChange( QtGui )
		QMAKE_POST_LINK += $$qtLibChange( QtCore )
		QMAKE_POST_LINK += $$qtLibChange( QtConcurrent )

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$BUNDLEDIR ) CFBundleExecutable "lib"$$TARGET".dylib"

		isEmpty( CASKBASE ) {
			QMAKE_POST_LINK += && $$FUGIO_ROOT/Fugio/mac_fix_libs_shared.sh $$BUNDLEDIR/Contents/MacOS
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
	plugin.path  = $$INSTALLDATA/plugins
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin

	libraries.path  = $$INSTALLDATA
	libraries.files = $$(QTDIR)/bin/Qt5OpenGL.dll

	INSTALLS += libraries
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
