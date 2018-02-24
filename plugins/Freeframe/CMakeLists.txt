cmake_minimum_required( VERSION 3.1 )

project( fugio-freeframe )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/freeframe/uuid.h

	freeframelibrary.cpp
	freeframelibrary.h
	freeframeplugin.cpp
	freeframeplugin.h

	FreeFrame.h
	FFGL.h

	ff10node.cpp
	ff10node.h
	ffglnode.cpp
	ffglnode.h

	settingswidget.cpp
	settingswidget.h
	settingswidget.ui
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} )

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
