cmake_minimum_required( VERSION 3.16 )

project( fugio LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
	plugins/pluginsform.cpp
	plugins/pluginsform.h
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

	inspector/inspectorform.h
	inspector/inspectorform.cpp
)

set( UIS_HDRS
	contextwidgetprivate.ui
	mainwindow.ui
	nodelistform.ui
	nodenamedialog.ui
	patchpromptdialog.ui
	performanceform.ui
	plugins/pluginsform.ui
	settings/settingsdialog.ui
	snippets/snippetsform.ui
	stylesheetform.ui
	inspector/inspectorform.ui
)

set( UI_RESOURCES fugio.rc )

add_definitions( -DFUGIOLIB_STATIC )

if( BUILD_PLUGINS )
	set( USE_LOCAL_PLUGINS TRUE )

	add_definitions( -DUSE_LOCAL_PLUGINS )
endif()

add_executable( ${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCE_FILES} ${UI_RESOURCES} ${UIS_HDRS} )

target_include_directories( ${PROJECT_NAME} PRIVATE ../fugiolib )

target_link_libraries( ${PROJECT_NAME} PRIVATE libfugio )

find_package( Qt6 REQUIRED COMPONENTS Core Concurrent Gui OpenGL Network Widgets Core5Compat
	OPTIONAL_COMPONENTS SerialPort WebSockets QuickWidgets QuickControls2 Quick Qml LinguistTools
)

qt_standard_project_setup( I18N_TRANSLATED_LANGUAGES de es fr it )

target_link_libraries( ${PROJECT_NAME} PUBLIC Qt::Core Qt::Concurrent Qt::Gui Qt::Network Qt::OpenGL Qt::Widgets )

install( TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${PATH_APP}
	BUNDLE DESTINATION ${PATH_APP}
)

#------------------------------------------------------------------------------
# Versions

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	FUGIO_APP_VERSION=${FUGIO_APP_VERSION}
	FUGIO_API_VERSION=${FUGIO_API_VERSION}
)

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

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
	OUTPUT_SCRIPT fugioapp-deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${fugioapp-deploy_script})

#------------------------------------------------------------------------------
# Raspberry Pi

# if( Q_OS_RASPBERRY_PI )
# 	target_include_directories( ${PROJECT_NAME} PRIVATE /opt/vc/include )

# 	find_library( BCM_HOST_LIBRARY NAMES bcm_host PATHS /opt/vc/lib )

# 	target_link_libraries( ${PROJECT_NAME} ${BCM_HOST_LIBRARY} )

# 	target_compile_definitions( ${PROJECT_NAME} PRIVATE OMX_SKIP64BIT )
# endif()
