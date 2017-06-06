cmake_minimum_required( VERSION 3.1 )

project( fugio-serial )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/serial/uuid.h
	
	deviceserial.cpp
	deviceserial.h
	deviceserialconfiguration.cpp
	deviceserialconfiguration.h
	serialinputnode.cpp
	serialinputnode.h
	serialoutputnode.cpp
	serialoutputnode.h
	serialplugin.cpp
	serialplugin.h
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
find_package( Qt5 COMPONENTS SerialPort )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

if( Qt5SerialPort_DIR )
	target_link_libraries( ${PROJECT_NAME} Qt5::SerialPort )

	add_definitions( -DSERIALPORT_SUPPORTED )
else( Qt5SerialPort_DIR )
	message( "Serial is not available" )
endif( Qt5SerialPort_DIR )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

