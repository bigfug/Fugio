cmake_minimum_required( VERSION 3.1 )

project( fugio-syphon )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/syphon/uuid.h

	opengl_includes.h
	syphonpin.cpp
	syphonpin.h
	syphonplugin.cpp
	syphonplugin.h
	syphonreceivernode.cpp
	syphonreceivernode.h
	syphonrendernode.cpp
	syphonrendernode.h
	syphonsendernode.cpp
	syphonsendernode.h
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

#------------------------------------------------------------------------------
# Syphon

if( APPLE )
	find_program( MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}" )

	set_target_properties( ${PROJECT_NAME} PROPERTIES INSTALL_RPATH “@loader_path/../Frameworks” )

	set( BUNDLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bundle" )

	find_library( Syphon_LIBRARY Syphon )
	find_library( Cocoa_LIBRARY Cocoa )
	find_library( Quartz_LIBRARY Quartz )

	if( Syphon_LIBRARY )
		message( "Syphon: YES" )

		target_sources( ${PROJECT_NAME} PRIVATE
			syphonsender.h
			syphonsender.mm
			syphonreceiver.h
			syphonreceiver.mm
		)

		target_link_libraries( ${PROJECT_NAME}
			${Syphon_LIBRARY}
			${Cocoa_LIBRARY}
			${Quartz_LIBRARY}
			)

		target_compile_definitions( ${PROJECT_NAME} PRIVATE SYPHON_SUPPORTED )

		add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${Syphon_LIBRARY}" "${BUNDLE_PATH}/Contents/Frameworks/Syphon.framework"
#			COMMAND install_name_tool -change "@loader_path/../Frameworks/Syphon.framework/Versions/A/Syphon" "@rpath/Syphon.framework/Versions/A/Syphon" "${BUNDLE_PATH}/Contents/MacOS/${PROJECT_NAME}"
			COMMENT "Bundling ${Syphon_LIBRARY}..."
			VERBATIM
		)
	else( Syphon_LIBRARY )
		message( "Syphon: NO" )
	endif( Syphon_LIBRARY )
else( APPLE )
	message( "Syphon: unsupported" )
endif( APPLE )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui OpenGL Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::OpenGL Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# OpenGL

find_package( OpenGL )

if( OpenGL_FOUND )
	target_include_directories( ${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR} )

	target_link_libraries( ${PROJECT_NAME} ${OPENGL_LIBRARIES} )
endif( OpenGL_FOUND )

