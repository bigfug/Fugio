cmake_minimum_required( VERSION 3.1 )

project( fugio-colour )

set( SOURCE_FILES
	../../include/fugio/colour/uuid.h
	../../include/fugio/colour/colour_interface.h
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	colourbutton.h
	colourpin.h
	colourplugin.h
	splitcolourhslanode.h
	splitcolourrgbanode.h
	joincolourrgbanode.h
	joincolourhslanode.h
	colourbuttonnode.h
	colourbutton.cpp
	colourpin.cpp
	colourplugin.cpp
	splitcolourhslanode.cpp
	splitcolourrgbanode.cpp
	joincolourrgbanode.cpp
	joincolourhslanode.cpp
	colourbuttonnode.cpp
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

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )
