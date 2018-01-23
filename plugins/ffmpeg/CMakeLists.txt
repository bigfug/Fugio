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
)

set( QRC_FILES
	resources.qrc
)

#—————————————————————————————————-

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} ${SNAPPY_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Install

if( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN}/ffmpeg )
else( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )
endif( WIN32 )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

#—————————————————————————————————-
# snappy

if( WIN32 )
	find_path( SNAPPY_DIR snappy-stubs-public.h )
	
	if( SNAPPY_DIR )
		target_include_directories( ${PROJECT_NAME} PRIVATE ${SNAPPY_DIR} )
		
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

		add_library( snappy STATIC ${SNAPPY_FILES} )
		
		target_compile_definitions( snappy PRIVATE SNAPPY_STATIC )

		set_target_properties( snappy PROPERTIES LINKER_LANGUAGE CXX )

		target_compile_definitions( ${PROJECT_NAME} PRIVATE SNAPPY_STATIC )

		target_link_libraries( ${PROJECT_NAME} snappy )

		target_sources( ${PROJECT_NAME} PRIVATE
			hap/source/hap.c
			hap/source/hap.h
		)

	endif( SNAPPY_DIR )
endif( WIN32 )

#—————————————————————————————————-
# snappy

find_package( SNAPPY QUIET )

if( SNAPPY_FOUND )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${SNAPPY_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${SNAPPY_LIBRARIES} )

	target_sources( ${PROJECT_NAME} PRIVATE
		hap/source/hap.c
		hap/source/hap.h
	)
else()
	message( "snappy not found" )
endif()

#—————————————————————————————————-
# ffmpeg

find_package( FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL )

if( FFMPEG_FOUND )
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
		
		file( GLOB dlls "${FFMPEG_LIBRARIES}/../bin/*.dll" )
		
		install( FILES ${dlls} DESTINATION "${PATH_PLUGIN}/ffmpeg" )

	endif( WIN32 )
else()
	message( "FFmpeg is not configured" )
endif()
