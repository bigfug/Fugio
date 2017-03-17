#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T10:18:40
#
#-------------------------------------------------

include( ../FugioGlobal.pri )

QT += network

greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets concurrent opengl

qtHaveModule( qml ) {
	QT += quickwidgets quickcontrols2 quick qml serialport
}

qtHaveModule( websockets ) {
	QT += websockets
}

TARGET = Fugio
TEMPLATE = app
CONFIG += c++11

#-------------------------------------------------
# Read in the Fugio version from version.txt

FUGIO_VERSION = "$$cat(version.txt)"

DEFINES += "FUGIO_VERSION=\"$$FUGIO_VERSION\""

# It would be great to use this with config.xml and package.xml too, at some point...

#-------------------------------------------------

SOURCES += main.cpp\
	mainwindow.cpp \
	app.cpp \
	linkitem.cpp \
	contextview.cpp \
	nodeitem.cpp \
	pinitem.cpp \
	contextwidgetprivate.cpp \
	stylesheetform.cpp \
	nodenamedialog.cpp \
	contextsubwindow.cpp \
	noteitem.cpp \
	wizards/nodeeditorwizard.cpp \
	contextwidget.cpp \
	wizards/firsttimewizard.cpp \
	performanceform.cpp \
	nodelistform.cpp \
	snippets/snippetsform.cpp \
	patchpromptdialog.cpp \
	model/contextmodel.cpp \
	model/nodemodel.cpp \
	model/basemodel.cpp \
	model/notemodel.cpp \
	model/pinmodel.cpp \
	model/pinlistmodel.cpp \
	model/baselistmodel.cpp \
	settings/settingsdialog.cpp

HEADERS  += mainwindow.h \
	app.h \
	linkitem.h \
	contextview.h \
	nodeitem.h \
	pinitem.h \
	undo/cmdnewcontext.h \
	undo/cmdnodeadd.h \
	../include/fugio/context_widget_interface.h \
	contextwidgetprivate.h \
	undo/cmdsetduration.h \
	undo/cmdsetdefaultvalue.h \
	undo/cmdsetupdatable.h \
	undo/cmdpinimport.h \
	undo/cmdpinremove.h \
	undo/cmdlinkadd.h \
	undo/cmdlinkremove.h \
	undo/cmdnoderename.h \
	undo/cmdnodesetcolour.h \
	undo/cmdnodemove.h \
	undo/cmdnodepinadd.h \
	undo/cmdnodesetactive.h \
	undo/cmdnoderemove.h \
	stylesheetform.h \
	undo/cmdnoteadd.h \
	undo/cmdremove.h \
	nodenamedialog.h \
	undo/cmdaddpinlink.h \
	contextsubwindow.h \
	noteitem.h \
	undo/cmdsetcolour.h \
	wizards/nodeeditorwizard.h \
	undo/cmdsetpinname.h \
	undo/cmdcontextviewpaste.h \
	undo/cmdcontextviewcut.h \
	undo/cmdmove.h \
	wizards/firsttimewizard.h \
	performanceform.h \
	nodelistform.h \
	../stable.h \
	snippets/snippetsform.h \
	undo/cmdgrouprename.h \
	undo/cmdgroup.h \
	undo/cmdungroup.h \
	undo/cmdgrouppush.h \
	undo/cmdgrouppop.h \
	undo/cmdpinsplit.h \
	undo/cmdpinjoin.h \
	../include/fugio/context_widget_signals.h \
	../include/fugio/utils.h \
	../include/fugio/global.h \
	undo/cmdgroupaddoutput.h \
	undo/cmdgroupaddinput.h \
	undo/cmdgroupreminput.h \
	undo/cmdgroupremoutput.h \
	patchpromptdialog.h \
	model/contextmodel.h \
	model/nodemodel.h \
	model/basemodel.h \
	model/notemodel.h \
	model/pinmodel.h \
	model/pinlistmodel.h \
	model/baselistmodel.h \
	settings/settingsdialog.h \
	undo/cmdpinmakeglobal.h \
	undo/cmdpinremoveglobal.h \
	undo/cmdpinconnectglobal.h \
	undo/cmdpindisconnectglobal.h

FORMS    += mainwindow.ui \
	contextwidgetprivate.ui \
	stylesheetform.ui \
	nodenamedialog.ui \
	performanceform.ui \
	nodelistform.ui \
	snippets/snippetsform.ui \
	patchpromptdialog.ui \
	settings/settingsdialog.ui

RC_FILE = fugio.rc

TRANSLATIONS = \
	$$FUGIO_BASE/translations/fugio_app_fr.ts \
	$$FUGIO_BASE/translations/fugio_app_es.ts

DISTFILES += \
	fugio-icon.ico \
	fugio.rc \
	Info.plist \
	package.xml \
	../config.osx.xml \
	../config.win.xml \
	about.html \
	version.txt \
	../installer/brew_install_update \
    ../installer/install_fugio

RESOURCES += \
	fugio.qrc

#------------------------------------------------------------------------------

CONFIG(release,debug|release) {
	examples.path = $$INSTALLDATA/examples
	examples.files = ../examples/*

	share.path = $$INSTALLDATA/share
	share.files = ../share/*

	snippets.path = $$INSTALLDATA/snippets
	snippets.files = ../snippets/*

	includes.path = $$INSTALLDATA/include
	includes.files = ../include/fugio

	INSTALLS += examples includes share snippets
}

macx {
	QMAKE_INFO_PLIST = Info.plist

	ICON = fugio.icns

	APP_DIR      = $$DESTDIR/$$TARGET".app"
	PLUGIN_DIR   = $$APP_DIR/Contents/PlugIns
	RESOURCE_DIR = $$APP_DIR/Contents/Resources

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += install_name_tool -change libfugio.1.dylib @executable_path/../../../libfugio.1.dylib $$APP_DIR/Contents/MacOS/Fugio

		QMAKE_POST_LINK += && macdeployqt $$APP_DIR -always-overwrite -qmldir=../qml

		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$APP_DIR ) CFBundleVersion \"$$FUGIO_VERSION\"
		QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$APP_DIR ) CFBundleGetInfoString \"$$FUGIO_VERSION\"

		QMAKE_POST_LINK += && install_name_tool -add_rpath @loader_path/../../../libs $$APP_DIR/Contents/MacOS/Fugio

		isEmpty( CASKBASE ) {
			installer_meta.path  = $$INSTALLROOT/meta
			installer_meta.files = $$_PRO_FILE_PWD_/package.xml

			installer_config.path  = $$INSTALLBASE/config
			installer_config.extra = cp $$_PRO_FILE_PWD_/../config.osx.xml $$installer_config.path/config.xml

			INSTALLS += installer_meta installer_config
		}

		app.path  = $$INSTALLDATA
		app.files = $$APP_DIR
		app.extra = cp $$(QTDIR)/plugins/platforms/libqcocoa.dylib $$PLUGIN_DIR/platforms

		INSTALLS += app
	}

	isEmpty( CASKBASE ) {
		brew_meta.path = $$INSTALLBASE/packages/sh.brew/meta
		brew_meta.files = ../installer/package.xml ../installer/installscript.qs

		brew_data.path = $$INSTALLBASE/packages/sh.brew/data
		brew_data.files = ../installer/brew_install_update

		INSTALLS += brew_meta brew_data
	}

	translation.path = $$app.path/$$TARGET".app"/Contents/translations
	translation.files = $$(QTDIR)/translations/qt*.qm

	INSTALLS += translation
}

windows {
	INSTALLDIR = $$INSTALLBASE/packages/com.bigfug.fugio

	installer_meta.path  = $$INSTALLROOT/meta
	installer_meta.files = $$_PRO_FILE_PWD_/package.xml

	installer_config.path  = $$INSTALLROOT/config
	installer_config.extra = cp $$_PRO_FILE_PWD_/../config.win.xml $$installer_config.path/config.xml

	INSTALLS += installer_meta installer_config

	translation.path = $$INSTALLDIR/data/plugins/translations
	translation.files = $$(QTDIR)/translations/qt*.qm

	INSTALLS += translation

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".exe" ) $$shell_path( $$INSTALLDIR/data/ )

		QMAKE_POST_LINK += & windeployqt --force --no-angle --no-opengl-sw -qmldir ../qml $$shell_path( $$INSTALLDIR/data )

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/include )
		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

		QMAKE_POST_LINK += & for %I in ( $$shell_path( $(QTDIR)/bin/Qt5Concurrent.dll ) ) do copy %I $$shell_path( $$INSTALLDIR/data/ )
	}
}

unix:!macx {
	DOLLAR = $

	QMAKE_LFLAGS += "-Wl,-rpath '-Wl,$${DOLLAR}$${DOLLAR}ORIGIN'"

#	contains( DEFINES, Q_OS_RASPBERRY_PI ) {
#		target.path = Desktop/Fugio

#		INSTALLS += target

#		examples.path  = Desktop/Fugio/examples
#		examples.files = $$_PRO_FILE_PWD_/../examples/*

#		INSTALLS += examples

#		snippets.path  = Desktop/Fugio/snippets
#		snippets.files = $$_PRO_FILE_PWD_/../snippets/*

#		INSTALLS += snippets

#		share.path  = Desktop/Fugio/share
#		share.files = $$_PRO_FILE_PWD_/../share/*

#		INSTALLS += share
#	}
}

#------------------------------------------------------------------------------
# Precompiled Headers

CONFIG += precompile_header

PRECOMPILED_HEADER = ../stable.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
	DEFINES += USING_PCH
}

#------------------------------------------------------------------------------
# Raspberry Pi

contains( DEFINES, Q_OS_RASPBERRY_PI ) {
	INCLUDEPATH += $$[QT_SYSROOT]/opt/vc/include $$[QT_SYSROOT]/opt/vc/include/interface/vcos/pthreads $$[QT_SYSROOT]/opt/vc/include/interface/vmcs_host/linux

	LIBS += -L$$[QT_SYSROOT]/opt/vc/lib -lbcm_host
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../include

win32:CONFIG(release, debug|release): LIBS += -L$$DESTDIR -lfugio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR -lfugiod
else:macx:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR -lfugio_debug
else:unix: LIBS += -L$$DESTDIR -lfugio

INCLUDEPATH += $$PWD/../FugioLib
DEPENDPATH += $$PWD/../FugioLib

#------------------------------------------------------------------------------
# General Unix/Linux/OS X (Brew) libs path

unix: {
	LIBS += -L/usr/local/lib
}

#------------------------------------------------------------------------------
# Python3

win32 {
	CONFIG(release,debug|release) {
		LIBS += -LC:/Python34/DLLs
	} else {
		LIBS += -LC:/Python-3.4.3/PCbuild
	}
}

#------------------------------------------------------------------------------
# Assimp

win32 {
	CONFIG(debug,debug|release) {
		LIBS += -L$$(LIBS)/assimp-3.3.1.32.2015/code/Debug
	} else {
		LIBS += -L$$(LIBS)/assimp-3.3.1.32.2015/code/Release
	}
}

#------------------------------------------------------------------------------
# portaudio

win32 {
	LIBS += -L$$(LIBS)/portaudio.32.2015/Release
}

#------------------------------------------------------------------------------
# OpenCV

win32 {
	CONFIG(debug,debug|release) {
		LIBS += -L$$(LIBS)/opencv-3.2.0/build/bin/Debug
	} else {
		LIBS += -L$$(LIBS)/opencv-3.2.0/build/bin/Release
	}
}

#------------------------------------------------------------------------------
# ffmpeg

win32 {
	LIBS += -L$$(LIBS)/ffmpeg-3.2-win32/lib
	LIBS += -L$$(LIBS)/ffmpeg-3.2-win32/bin
}

#------------------------------------------------------------------------------
# portmidi

windows {
	LIBS += -L$$(LIBS)/portmidi.32.2015/Release
}

#------------------------------------------------------------------------------
# Leap

win32 {
	LIBS += -L$$(LIBS)/LeapSDK/lib/x86
}

#------------------------------------------------------------------------------
# fugStream

win32 {
	CONFIG(debug,debug|release) {
		LIBS += -L../../../freeframe/freeframe-debug
	} else {
		LIBS += -L../../../freeframe/freeframe-release
	}
}

#------------------------------------------------------------------------------
# fugStream

win32 {
	LIBS += -L$$(LIBS)/fftw-3.3.5
}

#------------------------------------------------------------------------------
# Bullet Physics

win32 {
#    LIBS += -L$$(LIBS)/bullet3-2.83.4/bin
}

#------------------------------------------------------------------------------
# Lua

win32:exists( $$(LIBS)/Lua-5.3.3 ) {
	LIBS += -L$$(LIBS)/Lua-5.3.3
}

#------------------------------------------------------------------------------
# TUIO

win32:exists( $$(LIBS)/TUIO11_CPP ) {
	CONFIG(debug,debug|release) {
	   LIBS += -L$$(LIBS)/TUIO11_CPP/Debug
	} else {
		LIBS += -L$$(LIBS)/TUIO11_CPP/Release
	}
}

#------------------------------------------------------------------------------
# Canon EOS

win32:exists( $$(LIBS)/EDSDKv0304W/Windows/EDSDK ) {
	LIBS += -L$$(LIBS)/EDSDKv0304W/Windows/EDSDK/Dll
}

win64:exists( $$(LIBS)/EDSDKv0304W/Windows/EDSDK_64 ) {
	LIBS += -L$$(LIBS)/EDSDKv0304W/Windows/EDSDK_64/Dll
}

#-------------------
# OpenVR

windows {
	OPENVR_DIR = $$(LIBS)/openvr-1.0.0
}

win64 {
	LIBS += -L$$OPENVR_DIR/bin/win64
}

win32 {
	LIBS += -L$$OPENVR_DIR/bin/win32
}

#------------------------------------------------------------------------------
# NDI

win32 {
	NDI_DIR = "C:/Program Files/NewTek/NewTek NDI SDK/Bin/x86"
}

win64 {
	NDI_DIR = "C:/Program Files/NewTek/NewTek NDI SDK/Bin/x64"
}

windows:exists( $$NDI_DIR ) {
	LIBS += -L$$NDI_DIR
}

#------------------------------------------------------------------------------
# OpenNI

windows:exists( "C:/Program Files (x86)/OpenNI2/Redist" ) {
	LIBS += -L"C:/Program Files (x86)/OpenNI2/Redist"
}

#------------------------------------------------------------------------------
# NiTE2

windows:exists( "C:/Program Files (x86)/PrimeSense/NiTE2/Redist" ) {
	LIBS += -L"C:/Program Files (x86)/PrimeSense/NiTE2/Redist"
}

#------------------------------------------------------------------------------
# V8

windows:exists( $$(LIBS)/v8/v8/include/v8.h ) {
	LIBS += -L$$(LIBS)/v8/v8/out.gn/ia32.release
}
