cmake_minimum_required( VERSION 3.1 )

project( fugio-portaudio )

#—————————————————————————————————-
# PortAudio

if( UNIX )

include( FindPkgConfig )

pkg_check_modules( PORTAUDIO portaudio-2.0 )

endif( UNIX )

if( PORTAUDIO_FOUND )
	include_directories( ${PORTAUDIO_INCLUDE_DIRS} )

	link_directories( ${PORTAUDIO_LIBRARY_DIRS} )
endif( PORTAUDIO_FOUND )

#—————————————————————————————————-

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	deviceportaudio.cpp
	deviceportaudio.h
	portaudioinputnode.cpp
	portaudioinputnode.h
	portaudiooutputnode.cpp
	portaudiooutputnode.h
	portaudioplugin.cpp
	portaudioplugin.h
)

set( QRC_FILES
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

if( PORTAUDIO_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${PORTAUDIO_LIBRARIES} )

	add_definitions( -DPORTAUDIO_SUPPORTED )

endif( PORTAUDIO_FOUND )

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

