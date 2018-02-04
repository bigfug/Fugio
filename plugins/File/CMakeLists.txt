cmake_minimum_required( VERSION 3.1 )

project( fugio-file )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/file/uuid.h
	../../include/fugio/file/filename_interface.h
	fileplugin.cpp
	filewatchernode.cpp
	filenamepin.cpp
	filenamenode.cpp
	loadnode.cpp
	filewatchernode.h
	filenamepin.h
	filenamenode.h
	loadnode.h
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
