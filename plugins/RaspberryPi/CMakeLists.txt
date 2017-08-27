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

#target_compile_definitions( ${PROJECT_NAME} PRIVATE PIGPIO_SUPPORTED )