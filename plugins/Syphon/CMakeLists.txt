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

set( QRC_FILES
	resources.qrc
)

#—————————————————————————————————- ---------------

find_package( glew REQUIRED )

include_directories( ${GLEW_INCLUDE_DIRS} )

set( LINK_LIBRARIES ${LINK_LIBRARIES} ${GLEW_LIBRARIES} )

#—————————————————————————————————-

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

set( LINK_LIBRARIES ${LINK_LIBRARIES} ${OPENGL_LIBRARIES} )

#------------------------------------------------------------------------------

if( APPLE )
	find_library( Syphon_LIBRARY Syphon )
	find_library( OpenGL_LIBRARY OpenGL )
	find_library( Cocoa_LIBRARY Cocoa )
	find_library( Quartz_LIBRARY Quartz )

	if( Syphon_LIBRARY )
		set( LINK_LIBRARIES ${LINK_LIBRARIES}
			${Syphon_LIBRARY}
			${OpenGL_LIBRARY}
			${Cocoa_LIBRARY}
			${Quartz_LIBRARY}
			)

		add_definitions( -DSYPHON_SUPPORTED )

		set( SOURCE_FILES ${SOURCE_FILES}
				syphonsender.h
				syphonsender.mm
				syphonreceiver.h
				syphonreceiver.mm
			)
	else( Syphon_LIBRARY )
		message( "Syphon is not configured" )
	endif( Syphon_LIBRARY )
else( APPLE )
	message( "Syphon is not supported on this platform" )
endif( APPLE )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

set( LINK_LIBRARIES ${LINK_LIBRARIES}
	Qt5::Core Qt5::Gui Qt5::Widgets
)

#------------------------------------------------------------------------------

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

target_link_libraries( ${PROJECT_NAME} ${LINK_LIBRARIES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

