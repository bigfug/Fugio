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
	resources.qrc
)

#—————————————————————————————————-

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

#—————————————————————————————————-
# Oculus Rift

if( WIN32 )
	find_path( OCULUS_INCLUDE_DIR OVR_CAPI.h )
	
	find_library( OCULUS_LIB NAMES LibOVR )

	if( OCULUS_INCLUDE_DIR AND OCULUS_LIB )
		target_include_directories( ${PROJECT_NAME} PRIVATE ${OCULUS_INCLUDE_DIR} )

		target_link_libraries( ${PROJECT_NAME} ${OCULUS_LIB} )

		target_compile_definitions( ${PROJECT_NAME} PRIVATE OCULUS_PLUGIN_SUPPORTED )
	endif()
else()
	message( "Oculus Rift is not supported on this platform" )
endif()

#------------------------------------------------------------------------------
# OpenGL

find_package( OpenGL REQUIRED )

target_include_directories( ${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR} )

target_link_libraries( ${PROJECT_NAME} ${OPENGL_LIBRARIES} )

#------------------------------------------------------------------------------
# GLEW

find_package( GLEW REQUIRED )

target_link_libraries( ${PROJECT_NAME} GLEW::GLEW )

