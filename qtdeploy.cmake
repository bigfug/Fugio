# Retrieve the absolute path to qmake and then use that path to find
# the binaries
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if( WIN32 )

find_program( WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}" )

install( CODE "execute_process( COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --serialport --websockets --compiler-runtime --no-angle --no-opengl-sw --force --verbose 2 --qmldir \"${CMAKE_SOURCE_DIR}/qml\" \"${CMAKE_INSTALL_PREFIX}/${PATH_APP}/${PROJECT_NAME}.exe\" )"
    COMMENT "Running windeployqt..."
)

endif( WIN32 )

if( APPLE )

find_program( MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}" )

set_target_properties( ${PROJECT_NAME} PROPERTIES INSTALL_RPATH “@loader_path/../Frameworks” )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${MACDEPLOYQT_EXECUTABLE}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../.."
        -always-overwrite
    COMMENT "Running macdeployqt..."
)

endif( APPLE )

