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

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#------------------------------------------------------------------------------
# PortMidi

find_package( PortMidi QUIET )

if( PortMidi_FOUND )
	target_link_libraries( ${PROJECT_NAME} PortMidi )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE PORTMIDI_SUPPORTED )
else()
	message( "PortMidi is not configured" )
endif()


