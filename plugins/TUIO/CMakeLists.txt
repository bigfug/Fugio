cmake_minimum_required( VERSION 3.1 )

project( fugio-tuio )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../plugins-include/fugio/tuio/uuid.h

	tuioplugin.cpp
	tuioplugin.h
	tuioclientnode.cpp
	tuioclientnode.h
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
# TUIO - https://www.tuio.org/

target_include_directories( ${PROJECT_NAME} PRIVATE TUIO11_CPP/TUIO )
target_include_directories( ${PROJECT_NAME} PRIVATE TUIO11_CPP/oscpack )

target_sources( ${PROJECT_NAME} PRIVATE
	TUIO11_CPP/oscpack/ip/IpEndpointName.cpp
	TUIO11_CPP/oscpack/ip/IpEndpointName.h
	TUIO11_CPP/oscpack/ip/NetworkingUtils.h
	TUIO11_CPP/oscpack/ip/PacketListener.h
	TUIO11_CPP/oscpack/ip/TimerListener.h
	TUIO11_CPP/oscpack/ip/UdpSocket.h

	TUIO11_CPP/oscpack/osc/MessageMappingOscPacketListener.h
	TUIO11_CPP/oscpack/osc/OscException.h
	TUIO11_CPP/oscpack/osc/OscHostEndianness.h
	TUIO11_CPP/oscpack/osc/OscOutboundPacketStream.cpp
	TUIO11_CPP/oscpack/osc/OscOutboundPacketStream.h
	TUIO11_CPP/oscpack/osc/OscPacketListener.h
	TUIO11_CPP/oscpack/osc/OscPrintReceivedElements.cpp
	TUIO11_CPP/oscpack/osc/OscPrintReceivedElements.h
	TUIO11_CPP/oscpack/osc/OscReceivedElements.cpp
	TUIO11_CPP/oscpack/osc/OscReceivedElements.h
	TUIO11_CPP/oscpack/osc/OscTypes.cpp
	TUIO11_CPP/oscpack/osc/OscTypes.h

	TUIO11_CPP/TUIO/FlashSender.cpp
	TUIO11_CPP/TUIO/FlashSender.h
	TUIO11_CPP/TUIO/LibExport.h
	TUIO11_CPP/TUIO/OneEuroFilter.cpp
	TUIO11_CPP/TUIO/OneEuroFilter.h
	TUIO11_CPP/TUIO/OscReceiver.cpp
	TUIO11_CPP/TUIO/OscReceiver.h
	TUIO11_CPP/TUIO/OscSender.h
	TUIO11_CPP/TUIO/TcpReceiver.cpp
	TUIO11_CPP/TUIO/TcpReceiver.h
	TUIO11_CPP/TUIO/TcpSender.cpp
	TUIO11_CPP/TUIO/TcpSender.h
	TUIO11_CPP/TUIO/TuioBlob.cpp
	TUIO11_CPP/TUIO/TuioBlob.h
	TUIO11_CPP/TUIO/TuioClient.cpp
	TUIO11_CPP/TUIO/TuioClient.h
	TUIO11_CPP/TUIO/TuioContainer.cpp
	TUIO11_CPP/TUIO/TuioContainer.h
	TUIO11_CPP/TUIO/TuioCursor.cpp
	TUIO11_CPP/TUIO/TuioCursor.h
	TUIO11_CPP/TUIO/TuioDispatcher.cpp
	TUIO11_CPP/TUIO/TuioDispatcher.h
	TUIO11_CPP/TUIO/TuioListener.h
	TUIO11_CPP/TUIO/TuioManager.cpp
	TUIO11_CPP/TUIO/TuioManager.h
	TUIO11_CPP/TUIO/TuioObject.cpp
	TUIO11_CPP/TUIO/TuioObject.h
	TUIO11_CPP/TUIO/TuioPoint.cpp
	TUIO11_CPP/TUIO/TuioPoint.h
	TUIO11_CPP/TUIO/TuioServer.cpp
	TUIO11_CPP/TUIO/TuioServer.h
	TUIO11_CPP/TUIO/TuioTime.cpp
	TUIO11_CPP/TUIO/TuioTime.h
	TUIO11_CPP/TUIO/UdpReceiver.cpp
	TUIO11_CPP/TUIO/UdpReceiver.h
	TUIO11_CPP/TUIO/UdpSender.cpp
	TUIO11_CPP/TUIO/UdpSender.h
	TUIO11_CPP/TUIO/WebSockSender.cpp
	TUIO11_CPP/TUIO/WebSockSender.h
)

if( WIN32 )
	target_sources( ${PROJECT_NAME} PRIVATE
		TUIO11_CPP/oscpack/ip/win32/NetworkingUtils.cpp
		TUIO11_CPP/oscpack/ip/win32/UdpSocket.cpp
		)

	target_link_libraries( ${PROJECT_NAME} Ws2_32 Winmm )

else()
	target_sources( ${PROJECT_NAME} PRIVATE
		TUIO11_CPP/oscpack/ip/posix/NetworkingUtils.cpp
		TUIO11_CPP/oscpack/ip/posix/UdpSocket.cpp
		)
endif()

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )
