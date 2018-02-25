cmake_minimum_required( VERSION 3.1 )

project( fugio-kinect )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/kinect/uuid.h
	
	interactionclient.h
	kinectconfigdialog.cpp
	kinectconfigdialog.h
	kinectnode.cpp
	kinectnode.h
	kinectplugin.cpp
	kinectplugin.h
	kinectconfigdialog.ui
)

set( QRC_FILES
#	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Kinect

if( WIN32 )
	find_path( KINECT_INCLUDE_DIR NuiApi.h )
	
	find_library( KINECT_LIB NAMES Kinect10 )

	if( KINECT_INCLUDE_DIR AND KINECT_LIB )
		message( "Kinect: YES" )

		include_directories( ${KINECT_INCLUDE_DIR} )

		target_link_libraries( ${PROJECT_NAME} ${KINECT_LIB} )

		target_compile_definitions( ${PROJECT_NAME} PRIVATE KINECT_SUPPORTED )
	else()
		message( "Kinect: NO" )
	endif()
else()
	message( "Kinect: unsupported" )
endif( WIN32 )

