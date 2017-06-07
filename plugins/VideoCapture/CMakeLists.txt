cmake_minimum_required( VERSION 3.1 )

project( fugio-videocapture )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/videocapture/uuid.h

	devicedialog.cpp
	devicedialog.h
	videocapturenode.cpp
	videocapturenode.h
	videocaptureplugin.cpp
	videocaptureplugin.h
	
	devicedialog.ui
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

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

message( "VideoCapture is not configured" )

#target_compile_definitions( ${PROJECT_NAME} PRIVATE 