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

#------------------------------------------------------------------------------

if( APPLE )
	find_library( Syphon_LIBRARY Syphon )
	find_library( Cocoa_LIBRARY Cocoa )
	find_library( Quartz_LIBRARY Quartz )

	if( Syphon_LIBRARY )
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

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#------------------------------------------------------------------------------

if( Syphon_LIBRARY )
	target_link_libraries( ${PROJECT_NAME}
		${Syphon_LIBRARY}
		${Cocoa_LIBRARY}
		${Quartz_LIBRARY}
		)

	target_compile_definitions( ${PROJECT_NAME} PRIVATE SYPHON_SUPPORTED )
endif( Syphon_LIBRARY )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui OpenGL Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::OpenGL Qt5::Widgets )

#------------------------------------------------------------------------------
# OpenGL

find_package( OpenGL REQUIRED )

target_include_directories( ${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR} )

target_link_libraries( ${PROJECT_NAME} ${OPENGL_LIBRARIES} )
