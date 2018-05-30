cmake_minimum_required( VERSION 3.1 )

project( fugio-json )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/json/uuid.h

	jsonplugin.cpp
	jsonplugin.h

	jsonquerynode.cpp
	jsonquerynode.h
	stringtojsonnode.cpp
	stringtojsonnode.h

	jsonpin.cpp
	jsonpin.h
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

