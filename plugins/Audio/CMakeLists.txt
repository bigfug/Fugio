cmake_minimum_required( VERSION 3.1 )

project( fugio-audio )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/audio/uuid.h
	../../include/fugio/audio/audio_producer_interface.h
	../../include/fugio/audio/audio_sample_format.h
	../../include/fugio/audio/fft_interface.h
	../../include/fugio/audio/audio_instance_base.h
	audioplugin.cpp
	audiomixernode.cpp
	audiopin.cpp
	fftpin.cpp
	signalnode.cpp
	centroidnode.cpp
	magnitudenode.cpp
	frequencybandsnode.cpp
	audiofilternode.cpp
	audiofilterdisplay.cpp
	vcfnode.cpp
	mononode.cpp
	ringmodulatornode.cpp
	audioplugin.h
	audiomixernode.h
	audiopin.h
	fftpin.h
	signalnode.h
	centroidnode.h
	magnitudenode.h
	frequencybandsnode.h
	audiofilternode.h
	audiofilterdisplay.h
	vcfnode.h
	mononode.h
	ringmodulatornode.h
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
