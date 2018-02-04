cmake_minimum_required( VERSION 3.1 )

project( fugio-raspberrypi )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/raspberrypi/uuid.h
	
	raspberrypiplugin.cpp
	raspberrypiplugin.h
	sourcenode.cpp
	sourcenode.h
	gpionode.cpp
	gpionode.h
	pwmnode.cpp
	pwmnode.h

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
# Raspberry Pi

if( Q_OS_RASPBERRY_PI )
	target_include_directories( ${PROJECT_NAME} PRIVATE
		/opt/vc/include
		/opt/vc/include/interface/vcos/pthreads
		/opt/vc/include/interface/vmcs_host/linux
		)

	find_library( BCM_HOST_LIBRARY NAMES bcm_host PATHS /opt/vc/lib )

	find_library( OPENMAX_LIBRARY NAMES openmaxil PATHS /opt/vc/lib )

	find_library( PIGPIO_LIBRARY NAMES pigpio )

	target_link_libraries( ${PROJECT_NAME} ${BCM_HOST_LIBRARY} ${PIGPIO_LIBRARY} ${OPENMAX_LIBRARY} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE OMX_SKIP64BIT )
endif()

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#target_compile_definitions( ${PROJECT_NAME} PRIVATE PIGPIO_SUPPORTED )
