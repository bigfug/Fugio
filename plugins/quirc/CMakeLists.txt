cmake_minimum_required( VERSION 3.1 )

project( fugio-quirc )

set( SOURCE_FILES
	../../plugins-include/fugio/quirc/uuid.h

	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	quirc/lib/decode.c
	quirc/lib/identify.c
	quirc/lib/quirc.c
	quirc/lib/quirc.h
	quirc/lib/quirc_internal.h
	quirc/lib/version_db.c

	quircplugin.cpp
	quircplugin.h

	qrscannernode.cpp
	qrscannernode.h
)

include_directories( quirc/lib )

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#-----------------------------------------------------------------------------
# Qt

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

