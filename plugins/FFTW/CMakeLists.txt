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

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

#—————————————————————————————————-
# fftw

if( WIN32 )
	find_path( FFTW_DIR fftw3.h )
	
	find_library( FFTW_LIB NAMES fftw3f-3 HINTS ${FFTW_DIR} )

	if( FFTW_DIR AND FFTW_LIB )
		include_directories( ${FFTW_DIR} )

		target_link_libraries( ${PROJECT_NAME} ${FFTW_LIB} )

		add_definitions( -DFFTW_SUPPORTED )
	endif( FFTW_DIR AND FFTW_LIB )
endif( WIN32 )