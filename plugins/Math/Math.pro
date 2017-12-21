#-------------------------------------------------
#
# Project created by QtCreator 2015-01-14T22:32:51
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

TARGET = $$qtLibraryTarget(fugio-math)
TEMPLATE = lib

CONFIG += plugin c++11

QT += gui widgets

DESTDIR = $$DESTDIR/plugins

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
	normalisenode.cpp \
	minmaxnode.cpp \
	vector4pin.cpp \
	joinvector4node.cpp \
	splitvector4node.cpp \
	matrixinversenode.cpp \
	matrixorthographicnode.cpp \
	ceilnode.cpp \
	floornode.cpp \
	roundnode.cpp \
	quaternionpin.cpp \
    mathexpressionnode.cpp \
    modulusnode.cpp \
    inttobitsnode.cpp \
    multiplexornode.cpp \
    bitstopinsnode.cpp \
    notbitsnode.cpp \
    nandbitsnode.cpp \
    andbitsnode.cpp \
    orbitsnode.cpp \
    xorbitsnode.cpp \
    flipflopnode.cpp \
    cosinedegreesnode.cpp \
    minnode.cpp \
    maxnode.cpp \
    pointtransformnode.cpp \
    pownode.cpp \
    numberarraynode.cpp

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
	normalisenode.h \
	minmaxnode.h \
	vector4pin.h \
	joinvector4node.h \
	splitvector4node.h \
	matrixinversenode.h \
	matrixorthographicnode.h \
	ceilnode.h \
	floornode.h \
	roundnode.h \
	quaternionpin.h \
    mathexpressionnode.h \
    modulusnode.h \
    inttobitsnode.h \
    multiplexornode.h \
    bitstopinsnode.h \
    notbitsnode.h \
    nandbitsnode.h \
    andbitsnode.h \
    orbitsnode.h \
    xorbitsnode.h \
    flipflopnode.h \
    cosinedegreesnode.h \
    minnode.h \
    maxnode.h \
    pointtransformnode.h \
    pownode.h \
    numberarraynode.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_math_de.ts \
	translations/fugio_math_es.ts \
	translations/fugio_math_fr.ts

#------------------------------------------------------------------------------
# OSX plugin bundle

macx {
	DEFINES += TARGET_OS_MAC
	CONFIG -= x86
	CONFIG += lib_bundle

	BUNDLEDIR    = $$DESTDIR/$$TARGET".bundle"
	INSTALLDEST  = $$INSTALLDATA/plugins
	INCLUDEDEST  = $$INSTALLDATA/include/fugio

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
			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins
		}

		plugin.path = $$INSTALLDEST
		plugin.files = $$BUNDLEDIR
		plugin.extra = rm -rf $$INSTALLDEST/$$TARGET".bundle"

		INSTALLS += plugin
	}
}

windows {
	plugin.path  = $$INSTALLDATA/plugins
	plugin.files = $$DESTDIR/$$TARGET".dll"

	INSTALLS += plugin

	QMAKE_CFLAGS += /bigobj
	QMAKE_CXXFLAGS += /bigobj
}

#------------------------------------------------------------------------------
# Linux

unix:!macx {
    target.path = $$INSTALLBASE/usr/lib/fugio

    INSTALLS += target
}

#------------------------------------------------------------------------------
# API

INCLUDEPATH += $$PWD/../../include

