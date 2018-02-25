cmake_minimum_required( VERSION 3.1 )

project( fugio-portmidi )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/portmidi/uuid.h
	
	devicemidi.cpp
	devicemidi.h
	portmidiinputnode.cpp
	portmidiinputnode.h
	portmidioutputnode.cpp
	portmidioutputnode.h
	portmidiplugin.cpp
	portmidiplugin.h
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
# PortMidi

find_package( PortMidi QUIET )

if( PORTMIDI_FOUND )
	message( "PortMidi: YES" )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${PORTMIDI_INCLUDE_DIR} ${PORTTIME_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${PORTMIDI_LIBRARIES} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE PORTMIDI_SUPPORTED )

	if( WIN32 )
		string( REPLACE ".lib" ".dll" PORTMIDI_DLL ${PORTMIDI_LIBRARY} )

		install( FILES ${PORTMIDI_DLL} DESTINATION ${PATH_APP} )
	endif( WIN32 )

	if( APPLE )
		find_library( CoreMidi_LIBRARY CoreMidi )
		find_library( CoreFoundation_LIBRARY CoreFoundation )
		find_library( CoreServices_LIBRARY CoreServices )
		find_library( CoreAudio_LIBRARY CoreAudio )

		target_link_libraries( ${PROJECT_NAME}
			${CoreMidi_LIBRARY}
			${CoreFoundation_LIBRARY}
			${CoreServices_LIBRARY}
			${CoreAudio_LIBRARY}
			)
	endif( APPLE )
else()
	message( "PortMidi: NO" )
endif()

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

if( APPLE )
	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND install_name_tool -change "libportmidi.dylib" "@loader_path/../Frameworks/libportmidi.dylib" "${BUNDLE_PATH}/Contents/MacOS/${PROJECT_NAME}"
		VERBATIM
		)
endif( APPLE )

