if( APPLE AND CMAKE_BUILD_TYPE STREQUAL Release )
	set( BUNDLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bundle" )

	get_filename_component( LUA_LIB ${LUA_LIBRARY} NAME )

	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND install_name_tool -change "${LUA_LIBRARY}" "${LUA_LIB}" "${BUNDLE_PATH}/Contents/MacOS/${PROJECT_NAME}"
		COMMAND install_name_tool -change "/usr/local/opt/lua/lib/${LUA_LIB}" "${LUA_LIB}" "${BUNDLE_PATH}/Contents/MacOS/${PROJECT_NAME}"
		VERBATIM
		)

	include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND install_name_tool -change "${LUA_LIB}" "@executable_path/../Frameworks/${LUA_LIB}" "${BUNDLE_PATH}/Contents/MacOS/${PROJECT_NAME}"
		COMMAND ${CMAKE_COMMAND} -E remove "${BUNDLE_PATH}/Contents/Frameworks/${LUA_LIB}"
		VERBATIM
		)
endif()
