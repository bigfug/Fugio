cmake_minimum_required( VERSION 3.1 )

project( fugio-videocapture )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/videocapture/uuid.h

	devicedialog.cpp
	devicedialog.h
	videocapturenode.cpp
	videocapturenode.h
	videocaptureplugin.cpp
	videocaptureplugin.h
	
	videocapturedevice.cpp
	videocapturedevice.h

	devicedialog.ui
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

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# PortMidi

find_package( videocapture QUIET )

if( VIDEOCAPTURE_FOUND )
	message( "VideoCapture: YES" )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${VIDEOCAPTURE_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${VIDEOCAPTURE_LIBRARIES} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE VIDEOCAPTURE_SUPPORTED )
else()
	message( "VideoCapture: NO" )
endif()

if( WIN32 )
	target_link_libraries( ${PROJECT_NAME} Shlwapi Ole32 Mfplat Mf Mfuuid Mfreadwrite )
endif( WIN32 )

if( APPLE )
	find_library( Cocoa_LIBRARY Cocoa )
	find_library( Carbon_LIBRARY Carbon )
	find_library( CoreMedia_LIBRARY CoreMedia )
	find_library( CoreVideo_LIBRARY CoreVideo )
	find_library( AVFoundation_LIBRARY AVFoundation )

	target_link_libraries( ${PROJECT_NAME}
		${Cocoa_LIBRARY}
		${Carbon_LIBRARY}
		${CoreMedia_LIBRARY}
		${CoreVideo_LIBRARY}
		${AVFoundation_LIBRARY}
		)
endif( APPLE )
