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
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

#------------------------------------------------------------------------------
# Install

if( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN}/portmidi )
else( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )
endif( WIN32 )

include( ${CMAKE_SOURCE_DIR}/mac_fix_bundle.cmake )

#------------------------------------------------------------------------------
# PortMidi

find_package( PortMidi QUIET )

if( PORTMIDI_FOUND )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${PORTMIDI_INCLUDE_DIR} ${PORTTIME_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${PORTMIDI_LIBRARIES} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE PORTMIDI_SUPPORTED )

	if( WIN32 )
		string( REPLACE ".lib" ".dll" PORTMIDI_DLL ${PORTMIDI_LIBRARY} )

		install( FILES ${PORTMIDI_DLL} DESTINATION ${PATH_PLUGIN}/portmidi )
	endif( WIN32 )
else()
	message( "PortMidi is not configured" )
endif()


