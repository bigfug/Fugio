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

#	resources.qrc
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# PortAudio

find_package( PortAudio )

if( PORTAUDIO_FOUND )
	message( "PortAudio: YES" )

	target_link_libraries( ${PROJECT_NAME} ${PORTAUDIO_LIBRARIES} )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${PORTAUDIO_INCLUDE_DIRS} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE PORTAUDIO_SUPPORTED )
else()
	message( "PortAudio: NO" )
endif( PORTAUDIO_FOUND )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

if( WIN32 )
	if( PORTAUDIO_FOUND )
		STRING( FIND ${PORTAUDIO_LIBRARY} "_static" IS_PORTAUDIO_STATIC )

		if( IS_PORTAUDIO_STATIC GREATER -1 )
			message( "PortAudio: static library detected" )
		else()
			string( REPLACE ".lib" ".dll" PORTAUDIO_DLL ${PORTAUDIO_LIBRARY} )

			install( FILES ${PORTAUDIO_DLL} DESTINATION ${PATH_APP} )
		endif()
	endif()
endif( WIN32 )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )
