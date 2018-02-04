cmake_minimum_required( VERSION 3.1 )

project( fugio-oculusrift )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/oculusrift/uuid.h

	deviceoculusrift.cpp
	deviceoculusrift.h
	oculusriftnode.cpp
	oculusriftnode.h
	oculusriftplugin.cpp
	oculusriftplugin.h
)

set( QRC_FILES
#	resources.qrc
)

#------------------------------------------------------------------------------

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui OpenGL Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::OpenGL Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Oculus Rift

if( WIN32 )
	find_path( OCULUS_INCLUDE_DIR OVR_CAPI.h )
	
	find_library( OCULUS_LIB_DEBUG   NAMES LibOVR )
	find_library( OCULUS_LIB_RELEASE NAMES LibOVR )

	if( OCULUS_INCLUDE_DIR AND OCULUS_LIB_DEBUG AND OCULUS_LIB_RELEASE )
		target_include_directories( ${PROJECT_NAME} PRIVATE ${OCULUS_INCLUDE_DIR} )

		target_link_libraries( ${PROJECT_NAME} debug ${OCULUS_LIB_DEBUG} optimized ${OCULUS_LIB_RELEASE} )

		target_compile_definitions( ${PROJECT_NAME} PRIVATE OCULUS_PLUGIN_SUPPORTED )
	endif()
else()
	message( "Oculus Rift is not supported on this platform" )
endif()
