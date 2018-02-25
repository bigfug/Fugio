cmake_minimum_required( VERSION 3.1 )

project( fugio-spout )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/spout/uuid.h

	opengl_includes.h
	spoutpin.cpp
	spoutpin.h
	spoutplugin.cpp
	spoutplugin.h
	spoutreceiverform.cpp
	spoutreceiverform.h
	spoutreceivernode.cpp
	spoutreceivernode.h
	spoutsendernode.cpp
	spoutsendernode.h
	
	spoutreceiverform.ui
)

set( QRC_FILES
#	resources.qrc
)

#------------------------------------------------------------------------------

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} ${SPOUT_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Spout

if( WIN32 )
	find_package( Spout QUIET )
endif( WIN32 )

if( SPOUT_FOUND )
	message( "Spout: YES" )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${SPOUT_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${SPOUT_LIBRARIES} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE SPOUT_SUPPORTED )

	string( REPLACE ".lib" ".dll" SPOUT_DLL ${SPOUT_LIBRARY} )

	install( FILES ${SPOUT_DLL} DESTINATION ${PATH_APP} )
elseif( WIN32 )
	message( "Spout: NO" )
endif()

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui OpenGL Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::OpenGL Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# OpenGL

find_package( OpenGL )

if( OpenGL_FOUND )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${OPENGL_LIBRARIES} )
endif( OpenGL_FOUND )

