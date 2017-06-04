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

if( WIN32 )
	find_path( OPENCV_INCLUDE_DIR opencv/cv.h )
	
	find_library( OPENCV_CORE_LIB NAMES opencv_core )
	find_library( OPENCV_HIGHGUI_LIB NAMES opencv_highgui )
	find_library( OPENCV_IMGPROC_LIB NAMES opencv_imgproc )
	find_library( OPENCV_OBJDETECT_LIB NAMES opencv_objdetect )
	find_library( OPENCV_PHOTO_LIB NAMES opencv_photo )
	find_library( OPENCV_VIDEOIO_LIB NAMES opencv_videoio )
	find_library( OPENCV_VIDEO_LIB NAMES opencv_video )
	
	if( OPENCV_INCLUDE_DIR )
		include_directories( ${OPENCV_INCLUDE_DIR} )

		target_link_libraries( ${PROJECT_NAME}
			${OPENCV_CORE_LIB}
			${OPENCV_HIGHGUI_LIB}
			${OPENCV_IMGPROC_LIB}
			${OPENCV_OBJDETECT_LIB}
			${OPENCV_PHOTO_LIB}
			${OPENCV_VIDEOIO_LIB}
			${OPENCV_VIDEO_LIB}
		)
		
		add_definitions( -DOPENCV_SUPPORTED)
	endif( OPENCV_INCLUDE_DIR )
endif( WIN32 )

