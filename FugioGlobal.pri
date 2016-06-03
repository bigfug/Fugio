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

DISTFILES += \
    $$PWD/config.win.xml

HEADERS += \
    $$PWD/include/fugio/core/list_interface.h
