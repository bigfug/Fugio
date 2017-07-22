#-------------------------------------------------
#
# Project created by QtCreator 2017-06-14T11:45:31
#
#-------------------------------------------------

include( ../FugioGlobal.pri )

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FugioTime
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    clientconsole.cpp

HEADERS += \
        mainwindow.h \
    clientconsole.h

FORMS += \
        mainwindow.ui

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../include

win32:CONFIG(release, debug|release): LIBS += -L$$DESTDIR -lfugio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR -lfugiod
else:unix:CONFIG(release, debug|release): LIBS += -L$$DESTDIR -lfugio
else:unix:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR -lfugio_debug

INCLUDEPATH += $$PWD/../FugioLib
DEPENDPATH += $$PWD/../FugioLib

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$DESTDIR/fugio.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$DESTDIR/fugiod.lib
else:unix:CONFIG(release, debug|release): PRE_TARGETDEPS += $$DESTDIR/libfugio.a
else:unix:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$DESTDIR/libfugio_debug.a
