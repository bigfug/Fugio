cmake_minimum_required( VERSION 3.1 )

project( fugio-core-tests )

include_directories( ../../../FugioLib )

set( SOURCE_FILES
	testcore.cpp
	testcore.h
	../../../include/fugio/test_helper.h
)

add_definitions( -DFUGIOLIB_STATIC )

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

find_package( Qt5 REQUIRED COMPONENTS Core Test )

target_link_libraries( ${PROJECT_NAME} libfugio Qt5::Core Qt5::Test )

add_test( ${PROJECT_NAME} ${PROJECT_NAME} )

