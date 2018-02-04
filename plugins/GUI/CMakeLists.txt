cmake_minimum_required( VERSION 3.1 )

project( fugio-gui )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/gui/uuid.h
	../../include/fugio/playhead_interface.h
	../../include/fugio/gui/keyboard_interface.h
	../../include/fugio/gui/input_events_interface.h

	floatnode.cpp
	buttonnode.cpp
	integernode.cpp
	lcdnumbernode.cpp
	keyboardnode.cpp
	lednode.cpp
	keyboarddialog.cpp
	guiplugin.cpp
	slidernode.cpp
	keyboardpin.cpp
	mainwindownode.cpp
	choicenode.cpp
	numbermonitorform.cpp
	numbermonitornode.cpp
	stringnode.cpp
	leditem.cpp
	dialnode.cpp
	dialwidget.cpp
	screennode.cpp
	inputeventspin.cpp
	mousenode.cpp
	floatnode.h
	buttonnode.h
	integernode.h
	lcdnumbernode.h
	numbermonitorform.h
	numbermonitornode.h
	keyboardnode.h
	lednode.h
	keyboarddialog.h
	guiplugin.h
	choicenode.h
	keyboardpin.h
	slidernode.h
	mainwindownode.h
	stringnode.h
	leditem.h
	dialnode.h
	dialwidget.h
	screennode.h
	inputeventspin.h
	mousenode.h
	labelnode.cpp
	labelnode.h

	numbermonitorform.ui
	keyboarddialog.ui
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
