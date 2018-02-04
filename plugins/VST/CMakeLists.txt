cmake_minimum_required( VERSION 3.1 )

project( fugio-vst )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/vst/uuid.h

	dirlocker.h
	usediids.cpp
#	vst2node.cpp
#	vst2node.h
	vst3node.cpp
	vst3node.h
	vstplugin.cpp
	vstplugin.h
	vstview.cpp
	vstview.h
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

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

#------------------------------------------------------------------------------
# VST3SDK

find_package( VST3SDK QUIET )

if( VST3SDK_FOUND )
	message( "VST: YES" )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${VST3SDK_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${VST3SDK_LIBRARIES} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE VST_SUPPORTED RELEASE )

	target_sources( ${PROJECT_NAME} PRIVATE
		"${VST3SDK_INCLUDE_DIR}/public.sdk/source/common/memorystream.h"
		"${VST3SDK_INCLUDE_DIR}/public.sdk/source/common/memorystream.cpp"
		"${VST3SDK_INCLUDE_DIR}/pluginterfaces/base/funknown.cpp"
		"${VST3SDK_INCLUDE_DIR}/pluginterfaces/base/funknown.h"
		)

else()
	message( "VST: NO" )
endif()

if( APPLE )
	find_library( CoreFoundation_LIBRARY CoreFoundation )

	target_link_libraries( ${PROJECT_NAME}
		${CoreFoundation_LIBRARY}
		)

endif( APPLE )
