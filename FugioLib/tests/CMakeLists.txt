cmake_minimum_required( VERSION 3.14 )

project( libfugiotests )

include_directories( .. )

add_definitions( -DFUGIOLIB_STATIC )

set( SOURCE_FILES
	testglobal.cpp
	testglobal.h
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

find_package(Qt6 COMPONENTS Core Test QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Test QUIET)
endif()

target_link_libraries( ${PROJECT_NAME} libfugio Qt::Core Qt::Test )

add_test( libfugio ${PROJECT_NAME} )

