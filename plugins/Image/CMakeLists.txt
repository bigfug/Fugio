cmake_minimum_required( VERSION 3.1 )

project( fugio-image )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/image/uuid.h
	../../include/fugio/image/image.h
	../../include/fugio/image/painter_interface.h

	colourmasknode.cpp
	colourmasknode.h
	grabscreennode.cpp
	grabscreennode.h
	imagefilternode.cpp
	imagefilternode.h
	imageloadernode.cpp
	imageloadernode.h
	imagenode.cpp
	imagenode.h
	imagepin.cpp
	imagepin.h
	imageplugin.cpp
	imageplugin.h
	imagepreview.cpp
	imagepreview.h
	imagepreviewnode.cpp
	imagepreviewnode.h
	imagesavenode.cpp
	imagesavenode.h
	imagesizenode.cpp
	imagesizenode.h
	painterpin.cpp
	painterpin.h
	painterwindow.cpp
	painterwindow.h
	painterwindownode.cpp
	painterwindownode.h
	scaleimagenode.cpp
	scaleimagenode.h
	copynode.cpp
	copynode.h
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
