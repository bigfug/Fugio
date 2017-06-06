cmake_minimum_required( VERSION 3.1 )

project( fugio-vst )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/vst/uuid.h

	dirlocker.h
	usediids.cpp
	vst2node.cpp
	vst2node.h
	vst3node.cpp
	vst3node.h
	vstplugin.cpp
	vstplugin.h
	vstview.cpp
	vstview.h
)

set( QRC_FILES
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

if( TRUE )
	message( "VST is not configured" )
endif( TRUE )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

