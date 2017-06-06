cmake_minimum_required( VERSION 3.1 )

project( fugio-ffmpeg )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/ffmpeg/uuid.h
	
	mediaplayervideopreview.cpp
	audiobuffer.cpp
	audiobuffer.h
	ffmpegplugin.cpp
	ffmpegplugin.h
	imageconvertnode.cpp
	imageconvertnode.h
	mediaaudioprocessor.cpp
	mediaaudioprocessor.h
	medianode.cpp
	medianode.h
	mediapreset/media360_2048.h
	mediapreset/mediaProxy.h
	mediapreset/mediaWav44.h
	mediapreset/mediaYouTube1080pWide.h
	mediapreset/mediaYouTube240p.h
	mediapreset/mediaYouTube240pWide.h
	mediapreset/mediaYouTube360pWide.h
	mediapreset/mediaYouTube480pWide.h
	mediapreset/mediaYouTube720pWide.h
	mediapreset/mediadvdpal.h
	mediapreset/mediapresetinterface.h
	mediapreset/mediapresetmanager.cpp
	mediapreset/mediapresetmanager.h
	mediapreset/mediapresetmp4.h
	mediaprocessornode.cpp
	mediaprocessornode.h
	mediarecorderform.cpp
	mediarecorderform.h
	mediarecordernode.cpp
	mediarecordernode.h
	mediasegment.cpp
	mediasegment.h
	processoraudiobuffer.h
	segmentinterface.h
	mediaplayervideopreview.h
	mediatimelinenode.cpp
	mediatimelinenode.h
	
	mediarecorderform.ui
	mediaplayervideopreview.ui
	
	hap/source/hap.c
	hap/source/hap.h
)

set( QRC_FILES
	resources.qrc
)

#—————————————————————————————————-
# snappy

if( WIN32 )
	find_path( SNAPPY_DIR snappy-stubs-public.h )
	
	if( SNAPPY_DIR )
		include_directories( ${SNAPPY_DIR} )
		
		set( SNAPPY_FILES
			${SNAPPY_DIR}/snappy.cc
			${SNAPPY_DIR}/snappy-c.cc
			${SNAPPY_DIR}/snappy-sinksource.cc
			${SNAPPY_DIR}/snappy-stubs-internal.cc
			${SNAPPY_DIR}/snappy.h
			${SNAPPY_DIR}/snappy-c.h
			${SNAPPY_DIR}/snappy-internal.h
			${SNAPPY_DIR}/snappy-sinksource.h
			${SNAPPY_DIR}/snappy-stubs-internal.h
			${SNAPPY_DIR}/snappy-stubs-public.h
		)
		
		add_definitions( -DSNAPPY_STATIC )
	endif( SNAPPY_DIR )
endif( WIN32 )

#—————————————————————————————————-
# snappy

if( NOT WIN32 )
	include( FindPkgConfig )

	if( PKG_CONFIG_FOUND )
		pkg_check_modules( snappy snappy REQUIRED )

		if( snappy_FOUND )
			include_directories( ${snappy_INCLUDE_DIRS} )

			link_directories( ${snappy_LIBRARY_DIRS} )
		endif( snappy_FOUND )
	endif( PKG_CONFIG_FOUND )
endif( NOT WIN32 )

#—————————————————————————————————-

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} ${SNAPPY_FILES} )

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
# ffmpeg

find_path( FFMPEG_INCLUDE_DIR libavcodec/avcodec.h )

if( FFMPEG_INCLUDE_DIR )
	include_directories( ${FFMPEG_INCLUDE_DIR} )
endif( FFMPEG_INCLUDE_DIR )

find_path( FFMPEG_LIBRARY_DIR NAMES lib )

find_library( FFMPEG_AVCODEC_LIBRARY NAMES avcodec HINTS ${FFMPEG_LIBRARY_DIR} )
find_library( FFMPEG_AVDEVICE_LIBRARY NAMES avdevice HINTS ${FFMPEG_LIBRARY_DIR} )
find_library( FFMPEG_AVFILTER_LIBRARY NAMES avfilter HINTS ${FFMPEG_LIBRARY_DIR} )
find_library( FFMPEG_AVFORMAT_LIBRARY NAMES avformat HINTS ${FFMPEG_LIBRARY_DIR} )
find_library( FFMPEG_AVUTIL_LIBRARY NAMES avutil HINTS ${FFMPEG_LIBRARY_DIR} )
find_library( FFMPEG_POSTPROC_LIBRARY NAMES postproc HINTS ${FFMPEG_LIBRARY_DIR} )
find_library( FFMPEG_SWRESAMPLE_LIBRARY NAMES swresample HINTS ${FFMPEG_LIBRARY_DIR} )
find_library( FFMPEG_SWSCALE_LIBRARY NAMES swscale HINTS ${FFMPEG_LIBRARY_DIR} )

if( FFMPEG_LIBRARY_DIR )
	target_link_libraries( ${PROJECT_NAME}
		${FFMPEG_AVCODEC_LIBRARY}
		${FFMPEG_AVDEVICE_LIBRARY}
		${FFMPEG_AVFILTER_LIBRARY}
		${FFMPEG_AVFORMAT_LIBRARY}
		${FFMPEG_AVUTIL_LIBRARY}
		${FFMPEG_POSTPROC_LIBRARY}
		${FFMPEG_SWRESAMPLE_LIBRARY}
		${FFMPEG_SWSCALE_LIBRARY}
	)
	
	add_definitions( -DFFMPEG_SUPPORTED )
endif( FFMPEG_LIBRARY_DIR )

if( WIN32 )
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
	SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
	SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	SET(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
	SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
	SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
	SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	SET(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
	SET(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
	SET(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
endif( WIN32 )

#—————————————————————————————————-
# snappy

if( NOT WIN32 AND snappy_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${snappy_LIBRARIES} )
endif( NOT WIN32 AND snappy_FOUND )

