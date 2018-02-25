cmake_minimum_required( VERSION 3.1 )

project( fugio-artoolkit )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/artoolkit/uuid.h
	../../include/fugio/artoolkit/param_interface.h
	artoolkitplugin.h
	paramloadnode.h
	parampin.h
	trackernode.h
	paramcameranode.h
	artoolkitplugin.cpp
	paramloadnode.cpp
	parampin.cpp
	trackernode.cpp
	paramcameranode.cpp
)

set( QRC_FILES
#	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------

find_package( ARToolKit )

if( ARTOOLKIT_FOUND )
	message( "ARToolKit: YES" )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE ${ARTOOLKIT_DEFINITIONS} )

	target_link_libraries( ${PROJECT_NAME} ${ARTOOLKIT_LIBRARIES} )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${ARTOOLKIT_INCLUDE_DIR} )

	if( MSVC14 )
		target_link_libraries( ${PROJECT_NAME} legacy_stdio_definitions )
	endif( MSVC14 )

	if( APPLE )
		find_library( CoreFoundation_LIBRARY CoreFoundation )
		find_library( AppKit_LIBRARY AppKit )
		find_library( Accelerate_LIBRARY Accelerate )

		target_link_libraries( ${PROJECT_NAME}
			${CoreFoundation_LIBRARY}
			${AppKit_LIBRARY}
			${Accelerate_LIBRARY}
			)
	endif( APPLE )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE ARTOOLKIT_SUPPORTED )
else()
	message( "ARToolKit: NO" )
endif()

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Lua

find_package( Lua 5.2 )

if( NOT LUA_FOUND )
	find_path( LUA_INCLUDE_DIR lua.hpp )

	find_library( LUA_LIBRARY
	  NAMES lua
	)

	set( LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR} )
	set( LUA_LIBRARIES ${LUA_LIBRARY} )

	if( LUA_INCLUDE_DIR AND LUA_LIBRARY )
		set( LUA_FOUND ON )
	endif()
endif()

if( LUA_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${LUA_LIBRARIES} )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE LUA_SUPPORTED )
endif( LUA_FOUND )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

if( ( WIN32 ) AND ( LUA_FOUND ) )
	string( REPLACE ".lib" ".dll" LUA_DLL ${LUA_LIBRARIES} )

	install( FILES ${LUA_DLL} DESTINATION ${PATH_APP} )
endif()

if( APPLE )
	include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundleWithLua.cmake )
endif( APPLE )
