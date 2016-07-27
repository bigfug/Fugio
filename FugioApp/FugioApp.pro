#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T10:18:40
#
#-------------------------------------------------

QT += core gui network multimedia multimediawidgets serialport qml quick widgets quickwidgets
QT -= opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets concurrent

TARGET = Fugio
TEMPLATE = app
CONFIG += c++11

#-------------------------------------------------
# Read in the Fugio version from version.txt

FUGIO_VERSION = "$$cat(version.txt)"

DEFINES += "FUGIO_VERSION=\"$$FUGIO_VERSION\""

# It would be great to use this with config.xml and package.xml too, at some point...

#-------------------------------------------------

CONFIG(debug,debug|release) {
	DESTDIR = $$OUT_PWD/../../deploy-debug-$$QMAKE_HOST.arch
} else {
	DESTDIR = $$OUT_PWD/../../deploy-release-$$QMAKE_HOST.arch
}

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
    snippits/snippitsform.cpp \
    patchpromptdialog.cpp \
    model/contextmodel.cpp \
    model/nodemodel.cpp \
    model/basemodel.cpp \
    model/notemodel.cpp \
    model/pinmodel.cpp \
    model/pinlistmodel.cpp \
    model/baselistmodel.cpp

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
    snippits/snippitsform.h \
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
    model/baselistmodel.h

FORMS    += mainwindow.ui \
    contextwidgetprivate.ui \
    stylesheetform.ui \
    nodenamedialog.ui \
    performanceform.ui \
    nodelistform.ui \
    snippits/snippitsform.ui \
    patchpromptdialog.ui

RC_FILE = fugio.rc

TRANSLATIONS = fugioapp_fr.ts

DISTFILES += \
    fugio-icon.ico \
    fugio.rc \
    Info.plist \
    package.xml \
    ../config.osx.xml \
    ../config.win.xml \
    about.html \
    version.txt

RESOURCES += \
    fugio.qrc

macx {
    QMAKE_INFO_PLIST = Info.plist

    ICON = fugio.icns

    APP_DIR      = $$DESTDIR/$$TARGET".app"
    PLUGIN_DIR   = $$APP_DIR/Contents/PlugIns
    RESOURCE_DIR = $$APP_DIR/Contents/Resources
    INSTALLBASE  = $$OUT_PWD/../../deploy-installer-$$QMAKE_HOST.arch
    INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += install_name_tool -change libfugio.1.dylib @executable_path/../../../libfugio.1.dylib $$APP_DIR/Contents/MacOS/Fugio

        QMAKE_POST_LINK += && macdeployqt $$APP_DIR -always-overwrite

        QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$APP_DIR ) CFBundleVersion \"$$FUGIO_VERSION\"
        QMAKE_POST_LINK += && defaults write $$absolute_path( "Contents/Info", $$APP_DIR ) CFBundleGetInfoString \"$$FUGIO_VERSION\"

        QMAKE_POST_LINK += && mkdir -pv $$PLUGIN_DIR/platforms

        QMAKE_POST_LINK += && cp $$(QTDIR)/plugins/platforms/libqcocoa.dylib $$PLUGIN_DIR/platforms

        QMAKE_POST_LINK += && mkdir -pv $$PLUGIN_DIR/examples

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLBASE/config

        QMAKE_POST_LINK += && cp -R $$_PRO_FILE_PWD_/../config.osx.xml $$INSTALLBASE/config/config.xml

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data

        QMAKE_POST_LINK += && rm -rf $$INSTALLDIR/data/$$TARGET".app"

        QMAKE_POST_LINK += && cp -R $$_PRO_FILE_PWD_/package.xml $$INSTALLDIR/meta
        QMAKE_POST_LINK += && cp -R $$APP_DIR $$INSTALLDIR/data

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data/include
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data/plugins
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data/snippets
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/data/examples

        QMAKE_POST_LINK += && cp -R $$_PRO_FILE_PWD_/../examples/* $$INSTALLDIR/data/examples
    }
}

windows {
    INSTALLBASE  = $$OUT_PWD/../../deploy-installer-$$QMAKE_HOST.arch
    INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

    CONFIG(release,debug|release) {
        QMAKE_POST_LINK += echo
        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLBASE/config )

        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$_PRO_FILE_PWD_/../config.win.xml ) $$shell_path( $$INSTALLBASE/config/config.xml )

        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/meta )
        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data )

        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$_PRO_FILE_PWD_/package.xml ) $$shell_path( $$INSTALLDIR/meta/ )
        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".exe" ) $$shell_path( $$INSTALLDIR/data/ )

        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/include )
        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )
        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/snippets )
        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/examples )
        QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/platforms )

        QMAKE_POST_LINK += & xcopy $$shell_path( $$_PRO_FILE_PWD_/../examples/* ) $$shell_path( $$INSTALLDIR/data/examples ) /f /s /y

        QMAKE_POST_LINK += & copy /V /Y $$shell_path( $(QTDIR)/plugins/platforms/qwindows.dll ) $$shell_path( $$INSTALLDIR/data/platforms )

        QMAKE_POST_LINK += & for %I in ( $$shell_path( $(QTDIR)/bin/icu*.dll ) $$shell_path( $(QTDIR)/bin/Qt5Concurrent.dll ) $$shell_path( $(QTDIR)/bin/Qt5Core.dll ) $$shell_path( $(QTDIR)/bin/Qt5Gui.dll ) $$shell_path( $(QTDIR)/bin/Qt5Widgets.dll ) $$shell_path( $(QTDIR)/bin/Qt5Network.dll ) ) do copy %I $$shell_path( $$INSTALLDIR/data/ )
    }
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

linux:!mac:exists( /opt/vc/include/bcm_host.h ) {
    DEFINES += Q_OS_RASPERRY_PI

    LIBS += -L/opt/vc/lib -lGLESv2 -lEGL
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
        LIBS += -L$$(LIBS)/Python-3.4.3/PCbuild
    }
}

#------------------------------------------------------------------------------
# Assimp

win32 {
    CONFIG(debug,debug|release) {
        LIBS += -L$$(LIBS)/assimp-32/code/Debug
    } else {
        LIBS += -L$$(LIBS)/assimp-32/code/Release
    }
}

#------------------------------------------------------------------------------
# portaudio

win32 {
    LIBS += -L$$(LIBS)/portaudio.32.2013/bin/Win32/Release
}

#------------------------------------------------------------------------------
# OpenCV

win32 {
    CONFIG(debug,debug|release) {
            LIBS += -L$$(LIBS)/opencv-3.1.0/build/bin/Debug
    } else {
            LIBS += -L$$(LIBS)/opencv-3.1.0/build/bin/Release
    }
}

#------------------------------------------------------------------------------
# ffmpeg

win32 {
    LIBS += -L$$(LIBS)/ffmpeg-3.0.1-32/lib
    LIBS += -L$$(LIBS)/ffmpeg-3.0.1-32/bin
}

#------------------------------------------------------------------------------
# portmidi

windows {
    LIBS += -L$$(LIBS)/portmidi.32.2013/Release
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
    LIBS += -L$$(LIBS)/fftw-3.3.4
}

#------------------------------------------------------------------------------
# Bullet Physics

win32 {
#    LIBS += -L$$(LIBS)/bullet3-2.83.4/bin
}

#------------------------------------------------------------------------------
# Lua

win32:exists( $$(LIBS)/Lua-5.3.2 ) {
    LIBS += -L$$(LIBS)/Lua-5.3.2
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
