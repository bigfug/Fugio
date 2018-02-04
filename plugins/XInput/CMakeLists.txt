cmake_minimum_required( VERSION 3.1 )

project( fugio-xinput )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/xinput/uuid.h

	xinputnode.cpp
	xinputnode.h
	xinputplugin.cpp
	xinputplugin.h
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

if( WIN32 )
	target_link_libraries( ${PROJECT_NAME} Xinput9_1_0 )
	
	target_compile_definitions( ${PROJECT_NAME} PRIVATE XINPUT_SUPPORTED )
else()
	message( "XInput is not supported on this platform" )
endif()
