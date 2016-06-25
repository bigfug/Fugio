#-------------------------------------------------
#
# Project created by QtCreator 2015-01-14T22:32:51
#
#-------------------------------------------------

TARGET = $$qtLibraryTarget(fugio-math)
TEMPLATE = lib

CONFIG += plugin c++11

QT += gui widgets

CONFIG(debug,debug|release) {
	DESTDIR = $$OUT_PWD/../../../deploy-debug-$$QMAKE_HOST.arch/plugins
} else {
	DESTDIR = $$OUT_PWD/../../../deploy-release-$$QMAKE_HOST.arch/plugins
}

include( ../../../Fugio/FugioGlobal.pri )

DEFINES += MATH_LIBRARY

SOURCES += mathplugin.cpp \
        matrixlookatnode.cpp \
	matrixperspectivenode.cpp \
	multiplynode.cpp \
    vector3node.cpp \
    matrix4node.cpp \
    matrix4pin.cpp \
    splitvector3.cpp \
    vector3pin.cpp \
    matrixrotatenode.cpp \
    matrixtranslatenode.cpp \
    addnode.cpp \
    dividenode.cpp \
    matrixscalenode.cpp \
    subtractnode.cpp \
    sindegreenode.cpp \
    sinradiannode.cpp \
    pinode.cpp \
    comparenumbersnode.cpp \
    andnode.cpp \
    dotproductnode.cpp \
    notnode.cpp \
    ornode.cpp \
    xornode.cpp \
    nandnode.cpp \
    nornode.cpp \
    xnornode.cpp \
    joinvector3.cpp \
    rotationfromvectorsnode.cpp \
    matrixmultiplynode.cpp \
    absnode.cpp \
    arccosnode.cpp \
    radianstodegreesnode.cpp \
    crossproducenode.cpp \
    normalisenode.cpp

HEADERS += mathplugin.h \
    ../../include/fugio/math/uuid.h \
    ../../include/fugio/nodecontrolbase.h \
    ../../include/fugio/pincontrolbase.h \
    ../../include/fugio/math/matrix_interface.h \
    matrixlookatnode.h \
    matrixperspectivenode.h \
    multiplynode.h \
    vector3node.h \
    matrix4node.h \
    matrix4pin.h \
    splitvector3.h \
    vector3pin.h \
    matrixtranslatenode.h \
    matrixrotatenode.h \
    addnode.h \
    dividenode.h \
    matrixscalenode.h \
    subtractnode.h \
    sindegreenode.h \
    sinradiannode.h \
    pinode.h \
    comparenumbersnode.h \
    andnode.h \
    dotproductnode.h \
    notnode.h \
    ornode.h \
    xornode.h \
    nandnode.h \
    nornode.h \
    xnornode.h \
    joinvector3.h \
    rotationfromvectorsnode.h \
    matrixmultiplynode.h \
    absnode.h \
    arccosnode.h \
    radianstodegreesnode.h \
    crossproducenode.h \
    normalisenode.h

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

        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDIR/meta
        QMAKE_POST_LINK += && mkdir -pv $$INSTALLDEST
        QMAKE_POST_LINK += && mkdir -pv $$INCLUDEDEST

        QMAKE_POST_LINK += && rm -rf $$INSTALLDEST/$$TARGET".bundle"

        QMAKE_POST_LINK += && cp -a $$BUNDLEDIR $$INSTALLDEST
    }
}

windows {
	INSTALLBASE  = $$OUT_PWD/../../../deploy-installer-$$QMAKE_HOST.arch
	INSTALLDIR   = $$INSTALLBASE/packages/com.bigfug.fugio

	CONFIG(release,debug|release) {
		QMAKE_POST_LINK += echo

		QMAKE_POST_LINK += & mkdir $$shell_path( $$INSTALLDIR/data/plugins )

		QMAKE_POST_LINK += & copy /V /Y $$shell_path( $$DESTDIR/$$TARGET".dll" ) $$shell_path( $$INSTALLDIR/data/plugins )
	}
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

