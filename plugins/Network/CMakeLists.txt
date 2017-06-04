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
	getnode.cpp
	getnode.h
	networkplugin.cpp
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
	websocketclientnode.cpp
	websocketclientnode.h
	websocketdataservernode.cpp
	websocketdataservernode.h
)

set( QRC_FILES
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Network Widgets REQUIRED )
find_package( Qt5 COMPONENTS WebSockets )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets Qt5::WebSockets )

