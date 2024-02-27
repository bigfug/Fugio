
find_package(Qt6 COMPONENTS Core Concurrent Gui OpenGL Network Widgets
    OPTIONAL_COMPONENTS SerialPort WebSockets QuickWidgets QuickControls2 Quick Qml LinguistTools
    QUIET
)

if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Concurrent Gui OpenGL Network Widgets
        OPTIONAL_COMPONENTS SerialPort WebSockets QuickWidgets QuickControls2 Quick Qml LinguistTools
        QUIET
    )
endif()

target_link_libraries( ${PROJECT_NAME} Qt::Core Qt::Concurrent Qt::Gui Qt::Network Qt::OpenGL Qt::Widgets )

if( Qt5SerialPort_DIR )
        message( "Qt::SerialPort: YES" )
        target_link_libraries( ${PROJECT_NAME} Qt::SerialPort )
endif()

if( Qt5Qml_DIR )
        message( "Qt::Qml: YES" )
        target_link_libraries( ${PROJECT_NAME} Qt::Qml )
endif()

if( Qt5Quick_DIR )
        message( "Qt::Quick: YES" )
        target_link_libraries( ${PROJECT_NAME} Qt::Quick )
endif()

if( Qt5QuickControls2_DIR )
        message( "Qt::QuickControls2: YES" )
        target_link_libraries( ${PROJECT_NAME} Qt::QuickControls2 )
endif()

if( Qt5QuickWidgets_DIR )
        message( "Qt::QuickWidgets: YES" )
        target_link_libraries( ${PROJECT_NAME} Qt::QuickWidgets )
endif()

if( Qt5WebSockets_DIR )
        message( "Qt::WebSockets: YES" )
        target_link_libraries( ${PROJECT_NAME} Qt::WebSockets )
endif()

# Retrieve the absolute path to qmake and then use that path to find
# the binaries
get_target_property(_qmake_executable Qt::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if( WIN32 AND CMAKE_BUILD_TYPE STREQUAL Release )
	find_program( WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}" )

	get_filename_component( ABS_BINARY_DIR "${CMAKE_INSTALL_PREFIX}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

	# Run windeployqt immediately after build
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	  COMMAND "${WINDEPLOYQT_EXECUTABLE}"
		--verbose 2
		--no-compiler-runtime
		--no-angle
		--no-opengl-sw
		--concurrent --opengl --serialport --websockets --network --qml --quick --quickwidgets
		--dir "${ABS_BINARY_DIR}/${PATH_APP}"
		--libdir "${ABS_BINARY_DIR}/${PATH_APP}"
		--plugindir "${ABS_BINARY_DIR}/${PATH_APP}"
		\"$<TARGET_FILE:${PROJECT_NAME}>\"
	)

	file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_$<CONFIG>_path"
		CONTENT "$<TARGET_FILE:${PROJECT_NAME}>"
	)

	# Before installation, run a series of commands that copy each of the Qt
	# runtime files to the appropriate directory for installation
	install( CODE
		"
		file(READ \"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_Release_path\" _file)
		execute_process(
			COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
			--dry-run
			--no-compiler-runtime
			--no-angle
			--no-opengl-sw
			--list mapping
			--concurrent --opengl --serialport --websockets --network --qml --quick --quickwidgets
			--dir \"${ABS_BINARY_DIR}/${PATH_APP}\"
			--libdir \"${ABS_BINARY_DIR}/${PATH_APP}\"
			--plugindir \"${ABS_BINARY_DIR}/${PATH_APP}\"
			\${_file}
			OUTPUT_VARIABLE _output
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		separate_arguments(_files WINDOWS_COMMAND \${_output})

		while(_files)
			list(GET _files 0 _src)
			list(GET _files 1 _dest)
			message( \${_src} )
			execute_process(
				COMMAND \"${CMAKE_COMMAND}\" -E
				copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/${PATH_APP}/\${_dest}\"
			)
			separate_arguments(_files WINDOWS_COMMAND \${_output})
			while(_files)
					list(GET _files 0 _src)
					list(GET _files 1 _dest)
					execute_process(
							COMMAND \"${CMAKE_COMMAND}\" -E
							copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/${PATH_APP}/\${_dest}\"
					)
					list(REMOVE_AT _files 0 1)
			endwhile()
		endwhile()
		"
	)

	# windeployqt doesn't work correctly with the system runtime libraries,
	# so we fall back to one of CMake's own modules for copying them over
	set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
	include(InstallRequiredSystemLibraries)
	foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
		get_filename_component(filename "${lib}" NAME)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E
				copy_if_different "${lib}" \"$<TARGET_FILE_DIR:${PROJECT_NAME}>\"
		)
	endforeach()

endif()

if( APPLE AND CMAKE_BUILD_TYPE STREQUAL Release AND FUGIO_BUILD_DIST )

	find_program( MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}" )

	set_target_properties( ${PROJECT_NAME} PROPERTIES INSTALL_RPATH “@loader_path/../Frameworks” )

	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND "${MACDEPLOYQT_EXECUTABLE}"
			"$<TARGET_FILE_DIR:${PROJECT_NAME}>/../.."
			-always-overwrite
		COMMENT "Running macdeployqt..."
	)

endif()
