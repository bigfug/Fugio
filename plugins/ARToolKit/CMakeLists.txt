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
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------

find_package( ARToolKit )

if( ARTOOLKIT_FOUND )
	target_link_libraries( ${PROJECT_NAME} ARToolKit )

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
	message( "ARToolKit is not configured" )
endif()

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

#-------------------------------------
# Lua

find_package( Lua 5.2 )

if( LUA_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${LUA_LIBRARIES} )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE LUA_SUPPORTED )
endif( LUA_FOUND )

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

if( ( WIN32 ) AND ( LUA_FOUND ) )
	string( REPLACE ".lib" ".dll" LUA_DLL ${LUA_LIBRARIES} )

	install( FILES ${LUA_DLL} DESTINATION ${PATH_APP} )
endif()
