cmake_minimum_required( VERSION 3.1 )

project( fugio-portaudio )

add_library( ${PROJECT_NAME} MODULE 
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/portaudio/uuid.h

	deviceportaudio.cpp
	deviceportaudio.h
	portaudioinputnode.cpp
	portaudioinputnode.h
	portaudiooutputnode.cpp
	portaudiooutputnode.h
	portaudioplugin.cpp
	portaudioplugin.h

	resources.qrc
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

#------------------------------------------------------------------------------
# PortAudio

find_package( PortAudio QUIET )

if( PortAudio_FOUND )
	target_link_libraries( ${PROJECT_NAME} PortAudio )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE PORTAUDIO_SUPPORTED )
endif( PortAudio_FOUND )

#------------------------------------------------------------------------------
# Install

if( WIN32 )
	if( PortAudio_FOUND )
		string( REPLACE ".lib" ".dll" PORTAUDIO_DLL ${PORTAUDIO_LIBRARY} )

		install( FILES ${PORTAUDIO_DLL} DESTINATION ${PATH_PLUGIN}/portaudio )
	endif()
	
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN}/portaudio )

else( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )
endif( WIN32 )
