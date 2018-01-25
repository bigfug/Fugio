if( APPLE )
	set( BUNDLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bundle" )

	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_SOURCE_DIR}/mac_fix_lib.sh ${BUNDLE_PATH} ${PROJECT_NAME}
		COMMAND ${CMAKE_SOURCE_DIR}/mac_fix_libs.sh ${BUNDLE_PATH}/Contents/Frameworks
		COMMENT "Running macdeployqt..."
		VERBATIM
		)
endif( APPLE )

