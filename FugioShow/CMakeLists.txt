cmake_minimum_required( VERSION 3.14 )

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
# Version

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	FUGIO_APP_VERSION=${FUGIO_APP_VERSION}
	FUGIO_API_VERSION=${FUGIO_API_VERSION}
)

#------------------------------------------------------------------------------
# Qt5

include( ${CMAKE_SOURCE_DIR}/cmake/DeployApp.cmake )

set( TS_FILES
	translations/translations_de.ts
	translations/translations_es.ts
	translations/translations_fr.ts
	translations/translations_it.ts
	)

qt_add_translations( ${PROJECT_NAME} TS_FILES ${TS_FILES} RESOURCE_PREFIX "/translations" )

#------------------------------------------------------------------------------
# Raspberry Pi

if( Q_OS_RASPBERRY_PI )
	target_include_directories( ${PROJECT_NAME} PRIVATE /opt/vc/include )

	find_library( BCM_HOST_LIBRARY NAMES bcm_host PATHS /opt/vc/lib )

	target_link_libraries( ${PROJECT_NAME} ${BCM_HOST_LIBRARY} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE OMX_SKIP64BIT )
endif()

