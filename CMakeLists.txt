cmake_minimum_required( VERSION 3.1 )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

project( Fugio )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Instruct CMake to run moc automatically when needed.
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

set( CMAKE_INCLUDE_CURRENT_DIR ON )

set( PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON )

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "*** Have SIZEOF void * = 8, so 64-bit")
    set( IS_64_BIT 1 )
else ()
    message(STATUS "*** SIZEOF void * != 8, so not 64-bit")
endif ()

# There must be a better way to do this...

if( EXISTS /usr/lib/arm-linux-gnueabihf )
	set( Q_OS_RASPBERRY_PI TRUE )

	add_definitions( -DQ_OS_RASPBERRY_PI )

	message( "RASPBERRY PI BUILD" )
endif()

#------------------------------------------------------------------------------
# Enable Unit Testing

include( CTest )

set( CTEST_OUTPUT_ON_FAILURE ON )

#------------------------------------------------------------------------------

file( READ FugioApp/version.txt FUGIO_VERSION )

string( STRIP ${FUGIO_VERSION} FUGIO_VERSION )

#------------------------------------------------------------------------------

set( FUGIO_INSTALL_TYPE "Normal" CACHE STRING "Type of build" )

set_property( CACHE FUGIO_INSTALL_TYPE PROPERTY STRINGS Normal QtIFW Debian )

message( STATUS "FUGIO_INSTALL_TYPE=${FUGIO_INSTALL_TYPE}" )

set( PATH_APP . )
set( PATH_INCLUDE . )
set( PATH_SHARE . )
set( PATH_PLUGIN . )

if( FUGIO_INSTALL_TYPE STREQUAL "Normal" )
	message( STATUS "Normal Install" )

	set( PATH_INCLUDE	include )
	set( PATH_PLUGIN	plugins )
endif()

if( FUGIO_INSTALL_TYPE STREQUAL "QtIFW" )
	message( STATUS "QtIFW Installer" )
	
	set( PATH_BASE		packages/com.bigfug.fugio )
	set( PATH_DATA		${PATH_BASE}/data )

	set( PATH_APP		${PATH_DATA} )
	set( PATH_INCLUDE	${PATH_DATA}/include )
	set( PATH_SHARE		${PATH_DATA} )
	set( PATH_PLUGIN	${PATH_DATA}/plugins )

	if( WIN32 )
		if( IS_64_BIT )
			set( FUGIO_REPOSITORY_URL "http://www.bigfug.com/packages/fugio-win64" )
		else()
			set( FUGIO_REPOSITORY_URL "http://www.bigfug.com/packages/fugio-win32" )
		endif()
	elseif( APPLE )
		set( FUGIO_REPOSITORY_URL "http://www.bigfug.com/packages/fugio-osx" )
	endif()
	
	configure_file( qtifw_config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/qtifw_config.cmake @ONLY ) 
	
	add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/package.xml 
		COMMAND ${CMAKE_COMMAND} -P 
			${CMAKE_CURRENT_BINARY_DIR}/qtifw_config.cmake 
		DEPENDS 
			${CMAKE_CURRENT_BINARY_DIR}/qtifw_config.cmake 
			${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.xml 
			${CMAKE_CURRENT_SOURCE_DIR}/package.cmake.xml
			FugioApp/version.txt
		COMMENT "Configuring QtIFW..." 
			VERBATIM ) 
  
	add_custom_target( configure_qtifw ALL
		DEPENDS
			${CMAKE_CURRENT_BINARY_DIR}/package.xml
			${CMAKE_CURRENT_BINARY_DIR}/config.xml
			FugioApp/version.txt
	) 
	
	install( FILES ${CMAKE_CURRENT_BINARY_DIR}/package.xml	DESTINATION ${PATH_BASE}/meta )
	install( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.xml	DESTINATION config )
endif()

if( FUGIO_INSTALL_TYPE STREQUAL "Debian" )
	message( STATUS "Debian Package" )

	set( PATH_APP		usr/bin )
	set( PATH_INCLUDE	usr/include )
	set( PATH_SHARE		usr/share/fugio )
	set( PATH_PLUGIN	usr/lib/fugio )
	
	install( FILES FugioApp/control			DESTINATION DEBIAN )
	install( FILES FugioApp/fugio.desktop	DESTINATION usr/share/applications )
	install( FILES FugioApp/fugio.png		DESTINATION usr/share/icons/hicolor/256x256/apps )
endif()

#------------------------------------------------------------------------------

add_subdirectory( FugioLib )
add_subdirectory( FugioApp )
add_subdirectory( FugioShow )
add_subdirectory( plugins )

#------------------------------------------------------------------------------

install( DIRECTORY include/fugio DESTINATION ${PATH_INCLUDE} )
install( DIRECTORY examples DESTINATION ${PATH_SHARE} )
install( DIRECTORY snippets DESTINATION ${PATH_SHARE} )
install( DIRECTORY FugioApp/stylesheets DESTINATION ${PATH_SHARE} )

#------------------------------------------------------------------------------
# CPack

INCLUDE( InstallRequiredSystemLibraries )

set( CPACK_PACKAGE_NAME "Fugio" )
set( CPACK_PACKAGE_VENDOR "bigfug.com" )
set( CPACK_PACKAGE_CONTACT "fugio@bigfug.com" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fugio is a free, cross platform, creative visual programming system" )
set( CPACK_PACKAGE_EXECUTABLES "Fugio;Fugio" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "Fugio" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Fugio" )
#set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" )
#set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\vcredist_x86.exe\\\" /q:a'" )
#set( CPACK_NSIS_HELP_LINK "http:\\\\\\\\wiki.bigfug.com" )

if( Q_OS_RASPBERRY_PI )
	set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf )
elseif( IS_64_BIT )
	set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE x86 )
else()
	set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386 )
endif()

if( Q_OS_RASPBERRY_PI )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5concurrent5, libqt5core5a, libqt5gui5, libqt5opengl5, libqt5network5, libqt5widgets5,
		libgles2-mesa, libicu57, libavcodec57, libavdevice57, libavformat57, libavutil55, libswscale4,
		libswresample2, libavfilter6, libfftw3-single3, libportmidi0, liblua5.3-0, libportaudio2, pigpio,
		libqt5serialport5" )
elseif( LINUX )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5concurrent5, libqt5core5a, libqt5gui5, libqt5opengl5, libqt5network5, libqt5widgets5,
		libicu57, libavcodec57, libavdevice57, libavformat57, libavutil55, libswscale4,
		libswresample2, libavfilter6, libfftw3-single3, libportmidi0, liblua5.3-0, libportaudio2,
		libqt5serialport5" )
endif()

if( APPLE )
	set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON )
endif()

string( REPLACE "." ";" FUGIO_VERSION_LIST ${FUGIO_VERSION} )

list( GET FUGIO_VERSION_LIST 0 CPACK_PACKAGE_VERSION_MAJOR )
list( GET FUGIO_VERSION_LIST 1 CPACK_PACKAGE_VERSION_MINOR )
list( GET FUGIO_VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH )

include( CPack )

add_custom_target( mypackage
  COMMAND ${CMAKE_CPACK_COMMAND}
  COMMAND ${CMAKE_COMMAND} -E echo "after packaging"
)
