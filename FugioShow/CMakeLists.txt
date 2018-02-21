cmake_minimum_required( VERSION 3.1 )

if( UNIX AND NOT APPLE )
	project( fugioshow LANGUAGES CXX )
else()
	project( FugioShow LANGUAGES CXX )
endif()

include_directories( ../FugioLib )

#------------------------------------------------------------------------------

set( SOURCE_FILES
	../include/fugio/app_helper.h
	../include/fugio/global.h
	../include/fugio/utils.h
	../stable.h
	
	main.cpp
)

set( UIS_HDRS
)

add_definitions( -DFUGIOLIB_STATIC )

add_executable( ${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCE_FILES} ${UI_RESOURCES} ${UIS_HDRS} ) 

target_link_libraries( ${PROJECT_NAME} libfugio )

if( APPLE )
	set_target_properties( ${PROJECT_NAME} PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER com.bigfug.fugio
		MACOSX_FRAMEWORK_INFO_PLIST Info.plist
	)
endif()

install( TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${PATH_APP}
	BUNDLE DESTINATION ${PATH_APP}
)

#------------------------------------------------------------------------------
# Read version.txt

file( READ ../FugioApp/version.txt FUGIO_VERSION )

string( STRIP ${FUGIO_VERSION} FUGIO_VERSION )

target_compile_definitions( ${PROJECT_NAME} PRIVATE FUGIO_VERSION=${FUGIO_VERSION} )

#------------------------------------------------------------------------------
# Qt5

include( ${CMAKE_SOURCE_DIR}/cmake/DeployApp.cmake )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Raspberry Pi

if( Q_OS_RASPBERRY_PI )
	target_include_directories( ${PROJECT_NAME} PRIVATE /opt/vc/include )

	find_library( BCM_HOST_LIBRARY NAMES bcm_host PATHS /opt/vc/lib )

	target_link_libraries( ${PROJECT_NAME} ${BCM_HOST_LIBRARY} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE OMX_SKIP64BIT )
endif()

