cmake_minimum_required( VERSION 2.8 )

project( Fugio )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Instruct CMake to run moc automatically when needed.
set( CMAKE_AUTOMOC ON )
SET( CMAKE_AUTOUIC ON )

set( CMAKE_INCLUDE_CURRENT_DIR ON )

add_subdirectory( FugioLib )
add_subdirectory( FugioApp )
