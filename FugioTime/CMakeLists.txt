cmake_minimum_required( VERSION 3.1 )

project( FugioTime LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

include_directories( ../FugioLib )

#------------------------------------------------------------------------------

set( SOURCE_FILES
	clientconsole.cpp
	clientconsole.h
	main.cpp
	mainwindow.cpp
	mainwindow.h
	timecast.cpp
	timecast.h
	timeserver.cpp
	timeserver.h
)

set( UIS_HDRS
	mainwindow.ui
)

add_definitions( -DFUGIOLIB_STATIC )

add_executable( ${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCE_FILES} ${UI_RESOURCES} ${UIS_HDRS} ) 

if( APPLE )
	set_target_properties( ${PROJECT_NAME} PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER com.bigfug.fugio
		MACOSX_FRAMEWORK_INFO_PLIST Info.plist
	)
endif()

install( TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION .
	BUNDLE DESTINATION .
)

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Network Widgets )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::Widgets libfugio )

include( ../qtdeploy.cmake )

