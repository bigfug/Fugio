cmake_minimum_required( VERSION 3.1 )

project( fugio-leapmotion )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/leapmotion/uuid.h
	../../include/fugio/leapmotion/hand_interface.h

	deviceleap.cpp
	deviceleap.h
	leaphandnode.cpp
	leaphandnode.h
	leaphandpin.cpp
	leaphandpin.h
	leapmotionplugin.cpp
	leapnode.cpp
	leapnode.h
	leaprectifynode.cpp
	leaprectifynode.h
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

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Leap Motion

if( WIN32 )
	find_path( LEAP_INCLUDE_DIR Leap.h )
	
	find_library( LEAP_LIB NAMES Leap )

	if( LEAP_INCLUDE_DIR AND LEAP_LIB )
		message( "LeapMotion: YES" )

		include_directories( ${LEAP_INCLUDE_DIR} )

		target_link_libraries( ${PROJECT_NAME} ${LEAP_LIB} )

		target_compile_definitions( ${PROJECT_NAME} PRIVATE LEAP_PLUGIN_SUPPORTED )

		string( REPLACE ".lib" ".dll" LEAP_DLL ${LEAP_LIB} )

		install( FILES ${LEAP_DLL} DESTINATION ${PATH_APP} )
	else()
		message( "LeapMotion: NO" )
	endif()
else( WIN32 )
	message( "LeapMotion: unsupported" )
endif( WIN32 )

