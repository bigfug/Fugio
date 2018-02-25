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
	simpleblobdetectornode.cpp
	simpleblobdetectornode.h
	videocapturenode.cpp
	videocapturenode.h

	absdiffnode.cpp
	absdiffnode.h
	countnonzeronode.cpp
	countnonzeronode.h
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

include( ${CMAKE_SOURCE_DIR}/cmake/MacFixBundle.cmake )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# OpenCV

find_package( OpenCV COMPONENTS core OPTIONAL_COMPONENTS imgproc features2d objdetect photo video )

if( OpenCV_FOUND )
	message( "OpenCV: YES" )

	target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE OPENCV_SUPPORTED )

	if( WIN32 )
		file( GLOB OpenCV_DLL "${OpenCV_INSTALL_PATH}/bin/Release/opencv_*${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll" )

		install( FILES ${OpenCV_DLL} DESTINATION "${PATH_APP}" )
	endif()
else()
	message( "OpenCV: NO" )
endif()

