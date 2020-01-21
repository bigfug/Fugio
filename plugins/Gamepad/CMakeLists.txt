cmake_minimum_required( VERSION 3.5 )

project( fugio-gamepad LANGUAGES CXX )

set( PLUGIN_NAME "gamepad" )
set( PLUGIN_DESCRIPTION "Gamepad support" )
set( PLUGIN_FILENAME "${PROJECT_NAME}" )
set( PLUGIN_UUID "843c26cc-a007-42e6-a8a6-140b8d649c9e" )
set( PLUGIN_RELEASE_VERSION "1.0" )
set( PLUGIN_RELEASE_DATE "2019-10-25" )
set( PLUGIN_AUTHOR_NAME  "Alex May" )
set( PLUGIN_AUTHOR_EMAIL "bigfug@gmail.com" )

#------------------------------------------------------------------------------

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

add_library( ${PROJECT_NAME} MODULE
	include/fugio/gamepad/gamepad.h

	src/gamepadplugin.cpp
	src/gamepadplugin.h
	src/gamepadnode.cpp
	src/gamepadnode.h

	config.h.in
	manifest.json.in
)

target_include_directories( ${PROJECT_NAME} PRIVATE include )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

install( FILES
	include/fugio/gamepad/gamepad.h

	DESTINATION include/fugio/gamepad
)

#------------------------------------------------------------------------------
# Plugin Config

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	@ONLY
)

#------------------------------------------------------------------------------
# Plugin Manifest

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/manifest.json.in
	${CMAKE_CURRENT_BINARY_DIR}/manifest.json
	@ONLY
)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json DESTINATION . )

#------------------------------------------------------------------------------
# Fugio API

set( FUGIO_API "" CACHE PATH "" )

if( FUGIO_API AND EXISTS ${FUGIO_API} )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${FUGIO_API} )

	target_sources( ${PROJECT_NAME} PRIVATE
		${FUGIO_API}/fugio/nodecontrolbase.h
		${FUGIO_API}/fugio/pincontrolbase.h
		)
else()
	message( "--- FUGIO_API NOT SET ---" )
endif()

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5
	COMPONENTS Core Widgets LinguistTools
	OPTIONAL_COMPONENTS Gamepad
	QUIET )

target_link_libraries( ${PROJECT_NAME} PRIVATE Qt5::Widgets )

if( Qt5Gamepad_FOUND )
	message( "Qt5::Gamepad: YES" )

	target_link_libraries( ${PROJECT_NAME} PRIVATE Qt5::Gamepad )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE GAMEPAD_SUPPORTED )
else( Qt5Gamepad_FOUND )
	message( "Qt5::Gamepad: NO" )
endif( Qt5Gamepad_FOUND )

#------------------------------------------------------------------------------
# Translations

set( TS_FILES
	translations/translations_de.ts
	)

qt5_create_translation( QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/SRC ${TS_FILES} OPTIONS "-silent" )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/translations/translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY )

qt5_add_resources( QRC_FILES ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc )

target_sources( ${PROJECT_NAME} PRIVATE
	${TS_FILES}
	${QM_FILES}
	${QRC_FILES}
	)

set_property( SOURCE ${CMAKE_CURRENT_BINARY_DIR}/qrc_translations.cpp PROPERTY SKIP_AUTOGEN ON )
