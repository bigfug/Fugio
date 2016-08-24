#------------------------------------------------------------------------------
# Raspberry Pi

contains( QMAKE_CXXFLAGS, -marm ) {
	DEFINES += Q_OS_RASPBERRY_PI

	QMAKE_TARGET.arch = "arm"
} else {
	QMAKE_TARGET.arch = $$QMAKE_HOST.arch
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

CONFIG(debug,debug|release) {
	DESTDIR = $$FUGIO_ROOT/deploy-debug-$$QMAKE_TARGET.arch
} else {
	DESTDIR = $$FUGIO_ROOT/deploy-release-$$QMAKE_TARGET.arch
}

INSTALLBASE = $$FUGIO_ROOT/deploy-installer-$$QMAKE_TARGET.arch
