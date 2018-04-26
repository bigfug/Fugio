cmake_minimum_required( VERSION 3.1 )

project( fugio-midi )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/midi/midi_input_interface.h
	../../include/fugio/midi/midi_interface.h
	../../include/fugio/midi/uuid.h
	
	channelinputnode.cpp
	channelinputnode.h
	channeloutputnode.cpp
	channeloutputnode.h
	frequencytonotenode.cpp
	frequencytonotenode.h
	intervalnode.cpp
	intervalnode.h
	mididecodernode.cpp
	mididecodernode.h
	midifileloadnode.cpp
	midifileloadnode.h
	midiinputhelpernode.cpp
	midiinputhelpernode.h
	midiinputpin.cpp
	midiinputpin.h
	midiinputsyncnode.cpp
	midiinputsyncnode.h
	midinotenode.cpp
	midinotenode.h
	midioutputhelpernode.cpp
	midioutputhelpernode.h
	midioutputnode.cpp
	midioutputnode.h
	midioutputpin.cpp
	midioutputpin.h
	midiplugin.cpp
	midiplugin.h
	midirotarynode.cpp
	midirotarynode.h
	notetofrequencynode.cpp
	notetofrequencynode.h
	scalenode.cpp
	scalenode.h
    import/midifile.cpp
    import/midifile.h
    miditimelinenode.cpp
    miditimelinenode.h
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

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )
