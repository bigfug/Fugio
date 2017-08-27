cmake_minimum_required( VERSION 3.1 )

project( fugio-spout )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/spout/uuid.h

	opengl_includes.h
	spoutpin.cpp
	spoutpin.h
	spoutplugin.cpp
	spoutplugin.h
	spoutreceiverform.cpp
	spoutreceiverform.h
	spoutreceivernode.cpp
	spoutreceivernode.h
	spoutsendernode.cpp
	spoutsendernode.h
	
	spoutreceiverform.ui
)

set( QRC_FILES
	resources.qrc
)

#—————————————————————————————————-

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} ${SPOUT_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------

if( WIN32 )
	find_path( SPOUT_DIR Spout.h )
	
	if( SPOUT_DIR )
		target_include_directories( ${PROJECT_NAME} PRIVATE ${SPOUT_DIR} )
		
		set( SPOUT_FILES
			${SPOUT_DIR}/SpoutCopy.cpp
			${SPOUT_DIR}/SpoutDirectX.cpp
			${SPOUT_DIR}/SpoutGLDXinterop.cpp
			${SPOUT_DIR}/SpoutGLextensions.cpp
			${SPOUT_DIR}/SpoutMemoryShare.cpp
			${SPOUT_DIR}/SpoutReceiver.cpp
			${SPOUT_DIR}/SpoutSDK.cpp
			${SPOUT_DIR}/SpoutSender.cpp
			${SPOUT_DIR}/SpoutSenderMemory.cpp
			${SPOUT_DIR}/SpoutSenderNames.cpp
			${SPOUT_DIR}/SpoutSharedMemory.cpp

			${SPOUT_DIR}/Spout.h
			${SPOUT_DIR}/SpoutCommon.h
			${SPOUT_DIR}/SpoutCopy.h
			${SPOUT_DIR}/SpoutDirectX.h
			${SPOUT_DIR}/SpoutGLDXinterop.h
			${SPOUT_DIR}/SpoutGLextensions.h
			${SPOUT_DIR}/SpoutMemoryShare.h
			${SPOUT_DIR}/SpoutReceiver.h
			${SPOUT_DIR}/SpoutSDK.h
			${SPOUT_DIR}/SpoutSender.h
			${SPOUT_DIR}/SpoutSenderMemory.h
			${SPOUT_DIR}/SpoutSenderNames.h
			${SPOUT_DIR}/SpoutSharedMemory.h
		)
		
		add_library( Spout STATIC ${SPOUT_FILES} )

		target_link_libraries( ${PROJECT_NAME} Spout )

		target_compile_definitions( ${PROJECT_NAME} PRIVATE SPOUT_SUPPORTED )
	else( SPOUT_DIR )
		message( "Spout is not configured" )
	endif( SPOUT_DIR )
else( WIN32 )
	message( "Spout is not supported on this platform" )
endif( WIN32 )

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui OpenGL Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::OpenGL Qt5::Widgets )

#------------------------------------------------------------------------------
# OpenGL

find_package( OpenGL REQUIRED )

target_include_directories( ${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR} )

target_link_libraries( ${PROJECT_NAME} ${OPENGL_LIBRARIES} )
