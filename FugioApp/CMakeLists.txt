cmake_minimum_required( VERSION 3.1 )

if( UNIX AND NOT APPLE )
	project( fugio LANGUAGES CXX )
else()
	project( Fugio LANGUAGES CXX )
endif()

include_directories( ../FugioLib )

#------------------------------------------------------------------------------

set( SOURCE_FILES
	../include/fugio/app_helper.h
	../include/fugio/context_widget_interface.h
	../include/fugio/context_widget_signals.h
	../include/fugio/editor_interface.h
	../include/fugio/editor_signals.h
	../include/fugio/global.h
	../include/fugio/utils.h
	../stable.h
	
	app.cpp
	app.h
	contextsubwindow.cpp
	contextsubwindow.h
	contextview.cpp
	contextview.h
	contextwidget.cpp
	contextwidgetprivate.cpp
	contextwidgetprivate.h
	linkitem.cpp
	linkitem.h
	main.cpp
	mainwindow.cpp
	mainwindow.h
	model/baselistmodel.cpp
	model/baselistmodel.h
	model/basemodel.cpp
	model/basemodel.h
	model/contextmodel.cpp
	model/contextmodel.h
	model/nodemodel.cpp
	model/nodemodel.h
	model/notemodel.cpp
	model/notemodel.h
	model/pinlistmodel.cpp
	model/pinlistmodel.h
	model/pinmodel.cpp
	model/pinmodel.h
	nodeitem.cpp
	nodeitem.h
	nodelistform.cpp
	nodelistform.h
	nodenamedialog.cpp
	nodenamedialog.h
	noteitem.cpp
	noteitem.h
	patchpromptdialog.cpp
	patchpromptdialog.h
	performanceform.cpp
	performanceform.h
	pinitem.cpp
	pinitem.h
	settings/settingsdialog.cpp
	settings/settingsdialog.h
	snippets/snippetsform.cpp
	snippets/snippetsform.h
	stylesheetform.cpp
	stylesheetform.h
	undo/cmdaddpinlink.h
	undo/cmdcontextviewcut.h
	undo/cmdcontextviewpaste.h
	undo/cmdgroup.h
	undo/cmdgroupaddinput.h
	undo/cmdgroupaddoutput.h
	undo/cmdgrouppop.h
	undo/cmdgrouppush.h
	undo/cmdgroupreminput.h
	undo/cmdgroupremoutput.h
	undo/cmdgrouprename.h
	undo/cmdlinkadd.h
	undo/cmdlinkremove.h
	undo/cmdmove.h
	undo/cmdnewcontext.h
	undo/cmdnodeadd.h
	undo/cmdnodemove.h
	undo/cmdnodepinadd.h
	undo/cmdnoderemove.h
	undo/cmdnoderename.h
	undo/cmdnodesetactive.h
	undo/cmdnodesetcolour.h
	undo/cmdnoteadd.h
	undo/cmdpinconnectglobal.h
	undo/cmdpindisconnectglobal.h
	undo/cmdpinimport.h
	undo/cmdpinjoin.h
	undo/cmdpinmakeglobal.h
	undo/cmdpinremove.h
	undo/cmdpinremoveglobal.h
	undo/cmdpinsplit.h
	undo/cmdremove.h
	undo/cmdsetalwaysupdate.h
	undo/cmdsetcolour.h
	undo/cmdsetdefaultvalue.h
	undo/cmdsetduration.h
	undo/cmdsetpinname.h
	undo/cmdsetupdatable.h
	undo/cmdungroup.h
	wizards/firsttimewizard.cpp
	wizards/firsttimewizard.h
	wizards/nodeeditorwizard.cpp
	wizards/nodeeditorwizard.h

	pinpopupfloat.cpp
	pinpopupfloat.h
)

set( UIS_HDRS
	contextwidgetprivate.ui
	mainwindow.ui
	nodelistform.ui
	nodenamedialog.ui
	patchpromptdialog.ui
	performanceform.ui
	settings/settingsdialog.ui
	snippets/snippetsform.ui
	stylesheetform.ui
)

set( UI_RESOURCES fugio.rc )

add_definitions( -DFUGIOLIB_STATIC )

add_executable( ${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCE_FILES} ${UI_RESOURCES} ${UIS_HDRS} )

target_link_libraries( ${PROJECT_NAME} libfugio )

install( TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${PATH_APP}
	BUNDLE DESTINATION ${PATH_APP}
)

#------------------------------------------------------------------------------
# Read version.txt

file( READ version.txt FUGIO_VERSION )

string( STRIP ${FUGIO_VERSION} FUGIO_VERSION )

target_compile_definitions( ${PROJECT_NAME} PRIVATE FUGIO_VERSION=${FUGIO_VERSION} )

#------------------------------------------------------------------------------
# Icon

if( APPLE )
	set_target_properties( ${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
		)

	set( MACOSX_BUNDLE_ICON_FILE fugio.icns )

	set( FUGIO_ICON ${CMAKE_CURRENT_SOURCE_DIR}/fugio.icns )

	set_source_files_properties( ${FUGIO_ICON} PROPERTIES
		MACOSX_PACKAGE_LOCATION "Resources"
		)

	target_sources( ${PROJECT_NAME} PRIVATE
		${FUGIO_ICON}
		)
endif( APPLE )


#------------------------------------------------------------------------------
# Qt5

include( ${CMAKE_SOURCE_DIR}/cmake/DeployApp.cmake )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

qt5_add_resources( QRC_FILES fugio.qrc )

target_sources( ${PROJECT_NAME} PRIVATE ${QRC_FILES} )

if( WIN32 AND CMAKE_BUILD_TYPE STREQUAL Release )
	include( Windeployqt )

	windeployqt( ${PROJECT_NAME} ${PATH_APP} )
endif()

#------------------------------------------------------------------------------
# Raspberry Pi

if( Q_OS_RASPBERRY_PI )
	target_include_directories( ${PROJECT_NAME} PRIVATE /opt/vc/include )

	find_library( BCM_HOST_LIBRARY NAMES bcm_host PATHS /opt/vc/lib )

	target_link_libraries( ${PROJECT_NAME} ${BCM_HOST_LIBRARY} )

	target_compile_definitions( ${PROJECT_NAME} PRIVATE OMX_SKIP64BIT )
endif()

#-------------------------------------
# Lua

if( APPLE AND CMAKE_BUILD_TYPE STREQUAL Release )
	find_package( Lua 5.2 )

	if( NOT LUA_FOUND )
		find_path( LUA_INCLUDE_DIR lua.hpp )

		find_library( LUA_LIBRARY
		  NAMES lua
		)

		set( LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR} )
		set( LUA_LIBRARIES ${LUA_LIBRARY} )

		if( LUA_INCLUDE_DIR AND LUA_LIBRARY )
			set( LUA_FOUND ON )
		endif()
	endif()

	get_filename_component( LUA_LIB ${LUA_LIBRARY} NAME )

	set( BUNDLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app" )

	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${LUA_LIBRARY}" "${BUNDLE_PATH}/Contents/Frameworks/"
		COMMAND install_name_tool -id ${LUA_LIB} "${BUNDLE_PATH}/Contents/Frameworks/${LUA_LIB}"
		VERBATIM
	)
endif( APPLE AND CMAKE_BUILD_TYPE STREQUAL Release )
