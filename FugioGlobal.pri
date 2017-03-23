#------------------------------------------------------------------------------
# Raspberry Pi

contains( QMAKE_CXXFLAGS, -march=armv8-a ) {
	DEFINES += Q_OS_RASPBERRY_PI
	QMAKE_TARGET.arch = "rpi3"
} else:contains( QMAKE_CXXFLAGS, -march=armv7-a ) {
	DEFINES += Q_OS_RASPBERRY_PI
	QMAKE_TARGET.arch = "rpi2"
} else:contains( QMAKE_CXXFLAGS, -march=armv6zk ) {
	DEFINES += Q_OS_RASPBERRY_PI
	QMAKE_TARGET.arch = "rpi1"
} else {
	QMAKE_TARGET.arch = $$QMAKE_HOST.arch
}

#------------------------------------------------------------------------------
# macOS/OS X

macx {
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}

#------------------------------------------------------------------------------
# Global build options

defineReplace( libChangeGlobal ) {
	return( && install_name_tool -change $$1 /usr/local/lib/$$1 $$LIBCHANGEDEST )
}

defineReplace( libChange ) {
	return( && install_name_tool -change @executable_path/../Frameworks/$$1 @loader_path/../Frameworks/$$1 $$LIBCHANGEDEST )
}

defineReplace( qtLibChange ) {
	return( && install_name_tool -change @rpath/$$1".framework"/Versions/5/$$1 @executable_path/../Frameworks/$$1".framework"/Versions/5/$$1 $$LIBCHANGEDEST )
}

#------------------------------------------------------------------------------
# Set FUGIO_ROOT

FUGIO_ROOT = $$clean_path( $$PWD/.. )
FUGIO_BASE = $$clean_path( $$PWD )

CASKBASE = $$(CASKBASE)

CONFIG(debug,debug|release) {
	DESTDIR = $$FUGIO_ROOT/deploy-debug-$$QMAKE_TARGET.arch
} else {
	DESTDIR = $$FUGIO_ROOT/deploy-release-$$QMAKE_TARGET.arch
}

contains( DEFINES, INTERNAL_BUILD ) {
	INSTALLBASE = $$FUGIO_ROOT/deploy-internal
	INSTALLROOT = $$INSTALLBASE
	INSTALLDATA = $$INSTALLROOT
} else:isEmpty( CASKBASE ) {
	INSTALLBASE = $$FUGIO_ROOT/deploy-installer-$$QMAKE_TARGET.arch
	INSTALLROOT = $$INSTALLBASE/packages/com.bigfug.fugio
	INSTALLDATA = $$INSTALLROOT/data
} else {
	INSTALLBASE = $$FUGIO_ROOT/deploy-cask
	INSTALLROOT = $$INSTALLBASE/Fugio
	INSTALLDATA = $$INSTALLROOT
}

