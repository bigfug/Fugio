cmake_minimum_required( VERSION 3.1 )

project( fugio-opencv )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/opencv/uuid.h

	addnode.cpp
	addnode.h
	backgroundsubtractionnode.cpp
	backgroundsubtractionnode.h
	cascadeclassifiernode.cpp
	cascadeclassifiernode.h
	converttonode.cpp
	converttonode.h
	devicevideocapture.cpp
	devicevideocapture.h
	dilatenode.cpp
	dilatenode.h
	distancetransformnode.cpp
	distancetransformnode.h
	equalizehistnode.cpp
	equalizehistnode.h
	erodenode.cpp
	erodenode.h
	findcontoursnode.cpp
	findcontoursnode.h
	flipnode.cpp
	flipnode.h
	grayscalenode.cpp
	grayscalenode.h
	houghlinesnode.cpp
	houghlinesnode.h
	imageconvertnode.cpp
	imageconvertnode.h
	imagehomographynode.cpp
	imagehomographynode.h
	imagethresholdnode.cpp
	imagethresholdnode.h
	inpaintnode.cpp
	inpaintnode.h
	inrangenode.cpp
	inrangenode.h
	momentsnode.cpp
	momentsnode.h
	opencvplugin.cpp
	opencvplugin.h
	resizenode.cpp
	resizenode.h
	videocapturenode.cpp
	videocapturenode.h
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

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

#—————————————————————————————————-
# OpenCV

find_package( OpenCV COMPONENTS core highgui imgproc objdetect photo video videoio )

if( OpenCV_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE OPENCV_SUPPORTED )
else()
	message( "OpenCV is not configured" )
endif()

