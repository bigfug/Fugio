cmake_minimum_required( VERSION 3.14 )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

project( Fugio )

set( FUGIO_APP_VERSION "4.0.0" )
set( FUGIO_API_VERSION "4.0.0" )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Instruct CMake to run moc automatically when needed.
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC OFF )

set( CMAKE_INCLUDE_CURRENT_DIR ON )

set( PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON )

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "*** Have SIZEOF void * = 8, so 64-bit")
    set( IS_64_BIT 1 )
else ()
    message(STATUS "*** SIZEOF void * != 8, so not 64-bit")
endif ()

set( FUGIO_BUILD_DIST FALSE CACHE BOOL "Build Fugio for binary distribution" )

# There must be a better way to do this...

if( EXISTS /opt/vc/include/bcm_host.h )
	set( Q_OS_RASPBERRY_PI TRUE )

	add_definitions( -DQ_OS_RASPBERRY_PI )

	message( "RASPBERRY PI BUILD" )
endif()

#------------------------------------------------------------------------------
# Enable Unit Testing

include( CTest )

set( CTEST_OUTPUT_ON_FAILURE ON )

#------------------------------------------------------------------------------

message( "FUGIO VERSION ${FUGIO_APP_VERSION}" )

#------------------------------------------------------------------------------

set( PATH_APP . )
set( PATH_INCLUDE include )
set( PATH_SHARE share )
set( PATH_PLUGIN plugins )

if( WIN32 )
	set( PATH_APP bin )
	set( CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1 )
endif( WIN32 )

if( UNIX AND NOT APPLE )
	set( PATH_APP		bin )
	set( PATH_SHARE		share/fugio )
	set( PATH_PLUGIN	lib/fugio )
	
	install( FILES FugioApp/fugio.desktop	DESTINATION share/applications )
	install( FILES FugioApp/fugio.png		DESTINATION share/icons/hicolor/256x256/apps )
endif()

set( FUGIO_API "${CMAKE_SOURCE_DIR}/include" CACHE PATH "" )

#------------------------------------------------------------------------------

set( FUGIO_UNIX_DEPENDENCIES )

add_subdirectory( FugioLib )
add_subdirectory( FugioApp )
add_subdirectory( FugioShow )
add_subdirectory( fugio-plugin-manager )

#------------------------------------------------------------------------------

install( DIRECTORY include/ DESTINATION ${PATH_INCLUDE} )
install( DIRECTORY share/ DESTINATION ${PATH_SHARE} )
install( DIRECTORY DESTINATION plugins)

#------------------------------------------------------------------------------
# CPack

set( CPACK_PACKAGE_NAME "Fugio" )
set( CPACK_PACKAGE_VENDOR "bigfug.com" )
set( CPACK_PACKAGE_CONTACT "fugio@bigfug.com" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fugio is a free, cross platform, creative visual programming system" )
set( CPACK_PACKAGE_EXECUTABLES "Fugio;Fugio" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "Fugio" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Fugio" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE" )

if( WIN32 AND NOT UNIX )
	set( CPACK_NSIS_EXECUTABLES_DIRECTORY "${PATH_APP}" )
	set( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON )
	#set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\vcredist_x86.exe\\\" /q:a'" )
	#set( CPACK_NSIS_HELP_LINK "http:\\\\\\\\wiki.bigfug.com" )
endif()

if( APPLE )
	set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON )
endif()

# Set the output package filename

set( CPACK_PACKAGE_FILE_NAME "Fugio-${FUGIO_APP_VERSION}" )

# If we're running on a UNIX system, try to ascertain the type and version

if( UNIX )
	execute_process( COMMAND lsb_release -is
		OUTPUT_VARIABLE UNIX_ID
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	execute_process( COMMAND lsb_release -rs
		OUTPUT_VARIABLE UNIX_RELEASE
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	execute_process( COMMAND lsb_release -cs
		OUTPUT_VARIABLE UNIX_CODENAME
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if( "${UNIX_ID}" STREQUAL "" )
		set( UNIX_ID "unknown" )
	else()
		message( "UNIX ID: ${UNIX_ID}" )
	endif()

	if( "${UNIX_RELEASE}" STREQUAL "" )
		set( UNIX_RELEASE "unknown" )
	else()
		message( "UNIX RELEASE: ${UNIX_RELEASE}" )
	endif()

	if( "${UNIX_CODENAME}" STREQUAL "" )
		set( UNIX_CODENAME "unknown" )
	else()
		message( "UNIX CODENAME: ${UNIX_CODENAME}" )
	endif()
endif( UNIX )

# Set the output package extension

if( WIN32 AND IS_64_BIT )
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-win64" )
elseif( WIN32 )
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-win32" )
elseif( APPLE )
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-macOS" )
elseif( UNIX AND IS_64_BIT )
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-${UNIX_CODENAME}-x64" )
elseif( UNIX )
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-${UNIX_CODENAME}-i386" )
endif()

if( Q_OS_RASPBERRY_PI )
	set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf )
elseif( IS_64_BIT )
	set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64 )
else()
	set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386 )
endif()

# The dependencies for various UNIX packages

if( UNIX AND NOT APPLE )
	set( CPACK_DEBIAN_PACKAGE_DEBUG ON )
	set( CPACK_DEB_COMPONENT_INSTALL ON )
	set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
	set( CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON )
endif()

# Extract the version for CPACK

string( REPLACE "." ";" FUGIO_VERSION_LIST ${FUGIO_APP_VERSION} )

list( GET FUGIO_VERSION_LIST 0 CPACK_PACKAGE_VERSION_MAJOR )
list( GET FUGIO_VERSION_LIST 1 CPACK_PACKAGE_VERSION_MINOR )
list( GET FUGIO_VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH )

include( CPack )
