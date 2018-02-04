cmake_minimum_required( VERSION 3.1 )

project( fugio-painter )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/painter/uuid.h
	../../include/fugio/painter/painter_helper.h
	painternode.cpp
	painternode.h
	painterplugin.cpp
	painterplugin.h
	pennode.cpp
	pennode.h
	brushnode.cpp
	brushnode.h
	circlenode.cpp
	circlenode.h
	clearnode.cpp
	clearnode.h
	drawimagenode.cpp
	drawimagenode.h
	fontnode.cpp
	fontnode.h
	fontpin.cpp
	fontpin.h
	rectnode.cpp
	rectnode.h
	switchnode.cpp
	switchnode.h
	textnode.cpp
	textnode.h
	fontmetricsnode.cpp
	fontmetricsnode.h
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

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )
