cmake_minimum_required( VERSION 3.1 )

project( fugio-ndi )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/ndi/uuid.h
	
	ndiplugin.cpp
	ndiplugin.h
	ndireceivenode.cpp
	ndireceivenode.h
	ndisendnode.cpp
	ndisendnode.h
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

if( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN}/ndi )
else( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )
endif( WIN32 )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

if( APPLE )
	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND install_name_tool -change "@rpath/libndi.dylib" "@loader_path/../Frameworks/libndi.dylib" "${BUNDLE_PATH}/Contents/MacOS/${PROJECT_NAME}"
		VERBATIM
		)
endif( APPLE )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# NDI

find_package( NDI QUIET )

if( NDI_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${NDI_LIBRARY} )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${NDI_INCLUDE_DIR} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE NDI_SUPPORTED )

	if( WIN32 )
		install( FILES ${NDI_DLL} DESTINATION ${PATH_PLUGIN}/ndi )
	endif( WIN32 )
endif( NDI_FOUND )
