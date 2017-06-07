cmake_minimum_required( VERSION 3.1 )

project( fugio-dlib )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/dlib/uuid.h
	dlibplugin.cpp
	dlibplugin.h
	facefeaturesnode.cpp
	facefeaturesnode.h
)

set( QRC_FILES
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

#—————————————————————————————————-
# dlib

if( WIN32 )
	find_path( DLIB_DIR dlib/all/source.cpp )
	
	find_library( DLIB_LIB NAMES dlib HINTS ${DLIB_DIR} )

	if( DLIB_DIR AND DLIB_LIB )
		target_include_directories( ${PROJECT_NAME} PRIVATE ${DLIB_DIR} )

		target_link_libraries( ${PROJECT_NAME} ${DLIB_LIB} )

		target_compile_definitions( ${PROJECT_NAME} PRIVATE DLIB_SUPPORTED )
	endif( DLIB_DIR AND DLIB_LIB )
else( WIN32 )
#find_package( dlib )
#
#if( dlib_LIBS )
#	target_link_libraries( ${PROJECT_NAME} ${dlib_LIBS} )
#
#	add_definitions( -DDLIB_SUPPORTED )
#endif()
endif( WIN32 )

