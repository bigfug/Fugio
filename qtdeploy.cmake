# Retrieve the absolute path to qmake and then use that path to find
# the binaries
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if( WINDOWS )

find_program( WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}" )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Running windeployqt..."
)

endif( WINDOWS )

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

