cmake_minimum_required( VERSION 3.1 )

project( fugio-qml )

set( PLUGIN_SRCS
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	qmlplugin.cpp
	qmlwidget.cpp
	qmlinterfacenode.cpp
	qmlnode.cpp
	qmlpin.cpp
	qmlplugin.h
	../../include/fugio/qml/uuid.h
	qmlwidget.h
	qmlinterfacenode.h
	qmlnode.h
	qmlpin.h
	syntaxhighlighterqml.cpp
	syntaxhighlighterqml.h
)

#-----------------------------------------------------------------------------

add_library( ${PROJECT_NAME} MODULE ${PLUGIN_SRCS} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Qml Quick QuickWidgets Gui Widgets LinguistTools OPTIONAL_COMPONENTS QuickControls2 QUIET )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Qml Qt5::Quick Qt5::QuickWidgets Qt5::Gui Qt5::Widgets )

if( Qt5QuickControls2_DIR )
		target_link_libraries( ${PROJECT_NAME} Qt5::QuickControls2 )
endif()

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

if( APPLE )
	set( QTQUICK_PLUGINS "${Qt5Quick_DIR}/../../../qml" )

	if( EXISTS ${QTQUICK_PLUGINS} )
		if( EXISTS ${QTQUICK_PLUGINS}/QtQuick )
			install( DIRECTORY ${QTQUICK_PLUGINS}/QtQuick DESTINATION share/qmlplugins )
		endif()

		if( EXISTS ${QTQUICK_PLUGINS}/QtQuick.2 )
			install( DIRECTORY ${QTQUICK_PLUGINS}/QtQuick.2 DESTINATION share/qmlplugins )
		endif()
	endif()
endif()
