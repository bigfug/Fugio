cmake_minimum_required( VERSION 3.1 )

project( fugio-syphon )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/syphon/uuid.h

	opengl_includes.h
	syphonpin.cpp
	syphonpin.h
	syphonplugin.cpp
	syphonplugin.h
	syphonreceivernode.cpp
	syphonreceivernode.h
	syphonrendernode.cpp
	syphonrendernode.h
	syphonsendernode.cpp
	syphonsendernode.h
)

if( APPLE )
	set( SOURCE_FILES ${SOURCE_FILES}
		syphonsender.h
		syphonsender.mm
		syphonreceiver.h
		syphonreceiver.mm
	)
endif( APPLE )

set( QRC_FILES
	resources.qrc
)

#—————————————————————————————————- ---------------

find_package( glew REQUIRED )

include_directories( ${GLEW_INCLUDE_DIRS} )

link_libraries( ${GLEW_LIBRARIES} )

#—————————————————————————————————-

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

link_libraries( ${OPENGL_LIBRARIES} )

#------------------------------------------------------------------------------

#—————————————————————————————————-

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} ${SPOUT_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

