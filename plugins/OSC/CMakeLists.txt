cmake_minimum_required( VERSION 3.1 )

project( fugio-osc )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/osc/join_interface.h
	../../include/fugio/osc/namespace_interface.h
	../../include/fugio/osc/split_interface.h
	../../include/fugio/osc/uuid.h
	
	bundlernode.cpp
	bundlernode.h
	decodernode.cpp
	decodernode.h
	encodernode.cpp
	encodernode.h
	joinnode.cpp
	joinnode.h
	joinpin.cpp
	joinpin.h
	namespacepin.cpp
	namespacepin.h
	opensoundcontrolplugin.cpp
	opensoundcontrolplugin.h
	splitnode.cpp
	splitnode.h
	splitpin.cpp
	splitpin.h
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

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )
