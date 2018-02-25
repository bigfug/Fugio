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
#	resources.qrc
)

#------------------------------------------------------------------------------

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} ${SNAPPY_FILES} )

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
# snappy

find_package( Snappy QUIET )

if( SNAPPY_FOUND )
	message( "snappy: YES" )

	target_include_directories( ${PROJECT_NAME} PRIVATE ${SNAPPY_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${SNAPPY_LIBRARIES} )

	target_sources( ${PROJECT_NAME} PRIVATE
		hap/source/hap.c
		hap/source/hap.h
	)

	if( WIN32 )
		string( REPLACE ".lib" ".dll" SNAPPY_DLL ${SNAPPY_LIBRARY} )

		install( FILES ${SNAPPY_DLL} DESTINATION ${PATH_APP} )
	endif( WIN32 )

else()
	message( "snappy: NO" )
endif()

#------------------------------------------------------------------------------
# ffmpeg

find_package( FFmpeg COMPONENTS AVCODEC AVDEVICE AVFORMAT AVUTIL SWSCALE SWRESAMPLE )

if( SNAPPY_FOUND AND FFMPEG_FOUND )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS} )

	target_link_libraries( ${PROJECT_NAME} ${FFMPEG_LIBRARIES} )

#	message( ${FFMPEG_LIBRARIES} )

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
		
		file( GLOB dlls "${FFMPEG_DIR}/bin/*.dll" )
		
		if( dlls )
			install( FILES ${dlls} DESTINATION ${PATH_APP} )
		endif()

	endif( WIN32 )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE FFMPEG_SUPPORTED )

	message( "FFMPEG: YES" )
else()
	message( "FFMPEG: NO" )
endif()
