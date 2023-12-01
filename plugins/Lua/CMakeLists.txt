cmake_minimum_required( VERSION 3.1 )

project( fugio-lua )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../plugins-include/fugio/lua/uuid.h
	../../plugins-include/fugio/lua/lua_interface.h
	luaplugin.h
	luanode.h
	syntaxhighlighterlua.h
	luaexnode.h
	luaexpin.h
	luaarray.h
	luaplugin.cpp
	luanode.cpp
	syntaxhighlighterlua.cpp
	luaexnode.cpp
	luaexpin.cpp
	luaarray.cpp
)

set( QRC_FILES
#	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#-------------------------------------
# Lua

if( LUA_FOUND )
	message( "Lua: YES" )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${LUA_LIBRARIES} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE LUA_SUPPORTED )
else()
	message( "Lua: NO" )
endif()

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
