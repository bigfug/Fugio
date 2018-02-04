cmake_minimum_required( VERSION 3.1 )

project( fugio-network )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	
	../../include/fugio/network/uuid.h

	cobsdecodenode.cpp
	cobsdecodenode.h
	cobsencodenode.cpp
	cobsencodenode.h
	crc32.cpp
	crc32.h
	getnode.cpp
	getnode.h
	networkplugin.cpp
	packetdecodenode.cpp
	packetdecodenode.h
	packetencodenode.cpp
	packetencodenode.h
	slipdecodenode.cpp
	slipdecodenode.h
	slipencodenode.cpp
	slipencodenode.h
	tcpreceivenode.cpp
	tcpreceivenode.h
	tcpreceiverawnode.cpp
	tcpreceiverawnode.h
	tcpsendnode.cpp
	tcpsendnode.h
	tcpsendrawnode.cpp
	tcpsendrawnode.h
	udpreceiverawnode.cpp
	udpreceiverawnode.h
	udpsendrawnode.cpp
	udpsendrawnode.h
	universereceivenode.cpp
	universereceivenode.h
	universesendnode.cpp
	universesendnode.h
	websocketclientnode.cpp
	websocketclientnode.h
	websocketdataservernode.cpp
	websocketdataservernode.h
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

find_package( Qt5 COMPONENTS Core Gui Network Widgets LinguistTools OPTIONAL_COMPONENTS WebSockets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets Qt5::WebSockets )

if( Qt5WebSockets_DIR )
	target_compile_definitions( ${PROJECT_NAME} PRIVATE WEBSOCKET_SUPPORTED )
endif()

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

