cmake_minimum_required( VERSION 3.1 )

find_path( ARTOOKIT5_INCLUDE_DIR NAMES AR/ar.h HINTS $ENV{ARTOOLKIT5_ROOT}/include )

find_library( ARTOOKIT5_LIBRARY NAMES AR HINTS
	$ENV{ARTOOLKIT5_ROOT}/lib/win32-i386
	$ENV{ARTOOLKIT5_ROOT}/lib/win64-x64
)

get_filename_component( ARTOOKIT5_LIBRARY_DIR ${ARTOOKIT5_LIBRARY} DIRECTORY  )

set( ARTOOKIT5_INCLUDE_DIRS
	${ARTOOKIT5_INCLUDE_DIR}
)
set( ARTOOKIT5_LIBRARIES
	${ARTOOKIT5_LIBRARY}
)

set( ARTOOKIT5_LIBRARY_DIRS
	${ARTOOKIT5_LIBRARY_DIR}
)

if( ARTOOKIT5_INCLUDE_DIRS AND ARTOOKIT5_LIBRARIES )
	set( ARTOOKIT5_FOUND TRUE )
endif ()

if( ARTOOKIT5_FOUND )
	include_directories( ${ARTOOKIT5_INCLUDE_DIRS} )

	link_directories( ${ARTOOKIT5_LIBRARY_DIRS} )
endif( ARTOOKIT5_FOUND )

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

if( ARTOOKIT5_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${ARTOOKIT5_LIBRARY_DIR}/AR.lib ${ARTOOKIT5_LIBRARY_DIR}/ARICP.lib legacy_stdio_definitions )

	add_definitions( -DARTOOLKIT_SUPPORTED )
endif( ARTOOKIT5_FOUND )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

#-------------------------------------
# Lua

find_package( Lua 5.3 )

if( LUA_FOUND )
	include_directories( ${LUA_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${LUA_LIBRARIES} )

	add_definitions( -DLUA_SUPPORTED )
endif()

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

if( WIN32 )
	string( REPLACE ".lib" ".dll" LUA_DLL ${LUA_LIBRARIES} )

	install( FILES ${LUA_DLL} DESTINATION . )
endif( WIN32 )
