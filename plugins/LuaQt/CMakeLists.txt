cmake_minimum_required( VERSION 3.1 )

project( fugio-luaqt )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/luaqt/uuid.h

	luabrush.cpp
	luabrush.h
	luacolor.cpp
	luacolor.h
	luafont.cpp
	luafont.h
	luafontmetrics.cpp
	luafontmetrics.h
	luagradient.cpp
	luagradient.h
	luaimage.cpp
	luaimage.h
	luajsonarray.cpp
	luajsonarray.h
	luajsondocument.cpp
	luajsondocument.h
	luajsonobject.cpp
	luajsonobject.h
	luamatrix4x4.cpp
	luamatrix4x4.h
	luapainter.cpp
	luapainter.h
	luapen.cpp
	luapen.h
	luapointf.cpp
	luapointf.h
	luaqtplugin.cpp
	luaqtplugin.h
	luaquaternion.cpp
	luaquaternion.h
	luarectf.cpp
	luarectf.h
	luasizef.cpp
	luasizef.h
	luatransform.cpp
	luatransform.h
	luavector3.cpp
	luavector3.h
)

set( QRC_FILES
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

#-------------------------------------
# Lua

find_package( Lua 5.2 )

if( LUA_FOUND )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${LUA_LIBRARIES} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE LUA_SUPPORTED )
else()
	message( "LuaQt/Lua not configured" )
endif()

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

if( WIN32 AND LUA_LIBRARIES )
	string( REPLACE ".lib" ".dll" LUA_DLL ${LUA_LIBRARIES} )

	install( FILES ${LUA_DLL} DESTINATION . )
endif( WIN32 AND LUA_LIBRARIES )
