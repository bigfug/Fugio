#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T09:48:22
#
#-------------------------------------------------

include( ../../FugioGlobal.pri )

QT += gui widgets

TARGET = $$qtLibraryTarget(fugio-core)
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

DESTDIR = $$DESTDIR/plugins

DEFINES += BASENODES_LIBRARY

SOURCES += \
	stringpin.cpp \
	integerpin.cpp \
	floatpin.cpp \
	triggerpin.cpp \
	boolpin.cpp \
	bytearraypin.cpp \
	numberrangenode.cpp \
	randomnumbernode.cpp \
	anytriggernode.cpp \
	ratecontrolnode.cpp \
	numberrangelimitnode.cpp \
	boolrangenode.cpp \
	togglenode.cpp \
	triggerrangenumbernode.cpp \
	outputrangenode.cpp \
	sizepin.cpp \
	splitsizenode.cpp \
	joinsizenode.cpp \
	arraypin.cpp \
	pointpin.cpp \
	size3dpin.cpp \
	coreplugin.cpp \
	splitlistnode.cpp \
	listpin.cpp \
	indexnode.cpp \
	booltotriggernode.cpp \
	signalnumbernode.cpp \
	envelopenode.cpp \
	numberspreadnode.cpp \
	choicepin.cpp \
	splitpointnode.cpp \
	variantpin.cpp \
	duplicatepinsnode.cpp \
	lastupdatedinputnode.cpp \
	counternode.cpp \
	stringlistpin.cpp \
	listsizenode.cpp \
	listindexnode.cpp \
	framedelaynode.cpp \
	typesizenode.cpp \
	bytearraylistpin.cpp \
	switchnode.cpp \
	floatthresholdnode.cpp \
	smoothnode.cpp \
	autorangenode.cpp \
	variantlistpin.cpp \
	arraylistpin.cpp \
	rectpin.cpp \
	getsizenode.cpp \
	triggerarraynode.cpp \
	triggerbooleannode.cpp \
	makearraynode.cpp \
	flipflopnode.cpp \
	loggernode.cpp \
    bitarraypin.cpp

HEADERS += \
	stringpin.h \
	integerpin.h \
	floatpin.h \
	../../include/fugio/core/uuid.h \
	../../include/fugio/playhead_interface.h \
	triggerpin.h \
	boolpin.h \
	../../include/fugio/nodecontrolbase.h \
	../../include/fugio/pincontrolbase.h \
	bytearraypin.h \
	numberrangenode.h \
	randomnumbernode.h \
	anytriggernode.h \
	ratecontrolnode.h \
	numberrangelimitnode.h \
	boolrangenode.h \
	togglenode.h \
	triggerrangenumbernode.h \
	outputrangenode.h \
	sizepin.h \
	splitsizenode.h \
	joinsizenode.h \
	arraypin.h \
	pointpin.h \
	size3dpin.h \
	coreplugin.h \
	splitlistnode.h \
	listpin.h \
	indexnode.h \
	booltotriggernode.h \
	signalnumbernode.h \
	envelopenode.h \
	numberspreadnode.h \
	../../include/fugio/core/variant_interface.h \
	../../include/fugio/core/array_interface.h \
	../../include/fugio/core/list_interface.h \
	../../include/fugio/core/struct_entry_interface.h \
	choicepin.h \
	splitpointnode.h \
	variantpin.h \
	duplicatepinsnode.h \
	lastupdatedinputnode.h \
	counternode.h \
	stringlistpin.h \
	listsizenode.h \
	listindexnode.h \
	framedelaynode.h \
	typesizenode.h \
	bytearraylistpin.h \
	switchnode.h \
	floatthresholdnode.h \
	smoothnode.h \
	autorangenode.h \
	variantlistpin.h \
	../../include/fugio/core/array_list_interface.h \
	arraylistpin.h \
	arraylistentry.h \
	rectpin.h \
	../../include/fugio/core/size_interface.h \
	getsizenode.h \
	triggerarraynode.h \
	triggerbooleannode.h \
	makearraynode.h \
	flipflopnode.h \
	loggernode.h \
    bitarraypin.h

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
	translations/fugio_core_de.ts \
	translations/fugio_core_es.ts \
	translations/fugio_core_fr.ts

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

#		isEmpty( CASKBASE ) {
#			QMAKE_POST_LINK += && macdeployqt $$BUNDLEDIR -always-overwrite -no-plugins
#		}

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

INCLUDEPATH += $$PWD/../../include
