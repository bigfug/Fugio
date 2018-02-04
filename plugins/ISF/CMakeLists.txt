cmake_minimum_required( VERSION 3.1 )

project( fugio-isf )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/isf/uuid.h

	isfplugin.cpp
	isfplugin.h
    isfnode.cpp
    isfnode.h
    opengl_includes.h
    settingsform.cpp
    settingsform.h
	settingsform.ui
)

set( QRC_FILES
#	resources.qrc
)

if( MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
endif( MSVC )

#------------------------------------------------------------------------------

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui OpenGL Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::OpenGL Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )
