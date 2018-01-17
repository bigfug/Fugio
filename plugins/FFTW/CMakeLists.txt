cmake_minimum_required( VERSION 3.1 )

project( fugio-fftw )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/fftw/uuid.h
	fftnode.cpp
	fftnode.h
	fftwplugin.cpp
	fftwplugin.h
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

if( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN}/fftw )
else( WIN32 )
	install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )
endif( WIN32 )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

#—————————————————————————————————-
# fftw

find_package( FFTW )

if( FFTW_FOUND )
	target_link_libraries( ${PROJECT_NAME} ${FFTW_F_LIB} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE FFTW_SUPPORTED )

	if( WIN32 )
		string( REPLACE ".lib" ".dll" FFTW_DLL ${FFTW_F_LIB} )

		install( FILES ${FFTW_DLL} DESTINATION ${PATH_PLUGIN}/fftw )
	endif( WIN32 )

else()
	message( "FFTW not configured" )
endif()
