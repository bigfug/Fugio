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
	linebuffernode.cpp
	linebuffernode.h
	linenumberarea.h
	numbertostringnode.cpp
	numbertostringnode.h
	regexpnode.cpp
	regexpnode.h
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

	texteditorform.ui
)

set( QRC_FILES
	resources.qrc
)

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

