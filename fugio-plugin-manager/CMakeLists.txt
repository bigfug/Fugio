cmake_minimum_required(VERSION 3.14)

project(fugio-plugin-manager LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network LinguistTools)

set(TS_FILES fugio-plugin-manager_de_DE.ts)

add_definitions( -DFUGIOLIB_STATIC )

add_executable(fugio-plugin-manager
  main.cpp

  ../FugioLib/pluginmanager.cpp
  ../FugioLib/pluginmanager.h

  ../libs/zip/zip.h
  ../libs/zip/zip.cpp
  ../libs/zip/miniz.h

  # ${TS_FILES}
)

target_link_libraries(fugio-plugin-manager PRIVATE
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
)

if(COMMAND qt_create_translation)
	qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
else()
	qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
endif()

include(GNUInstallDirs)
install(TARGETS fugio-plugin-manager
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories( ${PROJECT_NAME} PRIVATE ../FugioLib )
