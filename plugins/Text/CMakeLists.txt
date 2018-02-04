cmake_minimum_required( VERSION 3.1 )

project( fugio-text )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/text/syntax_highlighter_interface.h
	../../include/fugio/text/syntax_error_interface.h
	../../include/fugio/text/syntax_error_signals.h
	../../include/fugio/text/uuid.h

	cmdtexteditorupdate.h
	codeeditor.cpp
	codeeditor.h
	containsnode.cpp
	containsnode.h
	linebuffernode.cpp
	linebuffernode.h
	linenumberarea.h
	leftnode.cpp
	leftnode.h
	midnode.cpp
	midnode.h
	numbertostringnode.cpp
	numbertostringnode.h
	regexpnode.cpp
	regexpnode.h
	rightnode.cpp
	rightnode.h
	stringjoinnode.cpp
	stringjoinnode.h
	syntaxerrorpin.cpp
	syntaxerrorpin.h
	texteditorform.cpp
	texteditorform.h
	texteditornode.cpp
	texteditornode.h
	texteditorremotenode.cpp
	texteditorremotenode.h
	textplugin.cpp
	textplugin.h
	stringsplitnode.cpp
	stringsplitnode.h
	stringtointegernode.cpp
	stringtointegernode.h
	stringtofloatnode.cpp
	stringtofloatnode.h
	chopnode.cpp
	chopnode.h

	texteditorform.ui
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
