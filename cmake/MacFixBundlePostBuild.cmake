
set( BUNDLE_LIB ${BUNDLE_PATH}/Contents/MacOS/${PROJECT_NAME} )

execute_process( COMMAND ${MACDEPLOYQT_EXECUTABLE} ${BUNDLE_PATH} -no-plugins -verbose=1 )

execute_process( COMMAND install_name_tool -id ${PROJECT_NAME} ${BUNDLE_LIB} )

file( GLOB QT_FRAMEWORKS "${BUNDLE_PATH}/Contents/Frameworks/Qt*.framework" )

foreach( QT_FRAMEWORK IN LISTS QT_FRAMEWORKS )
	file( REMOVE_RECURSE ${QT_FRAMEWORK} )
endforeach()

function( fixlibs LIB_PATH )
	message( "Processing dependecies of ${LIB_PATH}..." )

	execute_process( COMMAND otool -L ${LIB_PATH}
		OUTPUT_VARIABLE OTOOL_OUTPUT
		)

	string( REGEX REPLACE ";" "\\\\;" OTOOL_OUTPUT "${OTOOL_OUTPUT}")
	string( REGEX REPLACE "\n" ";" OTOOL_OUTPUT "${OTOOL_OUTPUT}")

	foreach( OTOOL_LINE IN LISTS OTOOL_OUTPUT )
		if( NOT OTOOL_LINE STREQUAL "" )
			string( STRIP ${OTOOL_LINE} OTOOL_LINE )

			string( REPLACE " (" ";" OTOOL_LINE "${OTOOL_LINE}" )

			list( LENGTH OTOOL_LINE OTOOL_LEN )

			if( OTOOL_LEN EQUAL 2 )
				list( GET OTOOL_LINE 0 OTOOL_LINE )

#				message( ${OTOOL_LINE} )

				if( OTOOL_LINE MATCHES "^(/usr/local/|/Users/|@executable_path/../Frameworks/lib)" )
					get_filename_component( OTOOL_LIB ${OTOOL_LINE} NAME )

					message( "Fixing ${OTOOL_LIB}..." )

					execute_process( COMMAND install_name_tool -change ${OTOOL_LINE} @loader_path/../Frameworks/${OTOOL_LIB} ${LIB_PATH} )
				endif()
			endif()
		endif()
	endforeach()
endfunction()

fixlibs( ${BUNDLE_LIB} )

file( GLOB DYLIB_LIST "${BUNDLE_PATH}/Contents/Frameworks/*.dylib" )

foreach( DYLIB IN LISTS DYLIB_LIST )
	fixlibs( ${DYLIB} )
endforeach()
