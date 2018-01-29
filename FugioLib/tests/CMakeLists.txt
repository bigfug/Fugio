cmake_minimum_required( VERSION 3.1 )

project( libfugiotests )

include_directories( .. )

add_definitions( -DFUGIOLIB_STATIC )

set( SOURCE_FILES
	testglobal.cpp
	testglobal.h
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

find_package( Qt5 REQUIRED COMPONENTS Core Test )

target_link_libraries( ${PROJECT_NAME} libfugio Qt5::Core Qt5::Test )

add_test( libfugio ${PROJECT_NAME} )

