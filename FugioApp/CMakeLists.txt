cmake_minimum_required( VERSION 2.8 )

project( FugioApp )

find_package( Qt5Core REQUIRED )
find_package( Qt5Concurrent REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5Widgets REQUIRED )

#Concurrent Network Widgets REQUIRED )

qt5_add_resources( UI_RESOURCES fugio.qrc )

include_directories( ../FugioLib )

set( SOURCE_FILES
	main.cpp
	mainwindow.cpp
	app.cpp
	linkitem.cpp
	contextview.cpp
	nodeitem.cpp
	pinitem.cpp
	contextwidgetprivate.cpp
	stylesheetform.cpp
	nodenamedialog.cpp
	contextsubwindow.cpp
	noteitem.cpp
	wizards/nodeeditorwizard.cpp
	contextwidget.cpp
	wizards/firsttimewizard.cpp
	performanceform.cpp
	nodelistform.cpp
	snippets/snippetsform.cpp
	patchpromptdialog.cpp
	model/contextmodel.cpp
	model/nodemodel.cpp
	model/basemodel.cpp
	model/notemodel.cpp
	model/pinmodel.cpp
	model/pinlistmodel.cpp
	model/baselistmodel.cpp
	settings/settingsdialog.cpp
	mainwindow.h
	app.h
	linkitem.h
	contextview.h
	nodeitem.h
	pinitem.h
	undo/cmdnewcontext.h
	undo/cmdnodeadd.h
	../include/fugio/context_widget_interface.h
	contextwidgetprivate.h
	undo/cmdsetduration.h
	undo/cmdsetdefaultvalue.h
	undo/cmdsetupdatable.h
	undo/cmdpinimport.h
	undo/cmdpinremove.h
	undo/cmdlinkadd.h
	undo/cmdlinkremove.h
	undo/cmdnoderename.h
	undo/cmdnodesetcolour.h
	undo/cmdnodemove.h
	undo/cmdnodepinadd.h
	undo/cmdnodesetactive.h
	undo/cmdnoderemove.h
	stylesheetform.h
	undo/cmdnoteadd.h
	undo/cmdremove.h
	nodenamedialog.h
	undo/cmdaddpinlink.h
	contextsubwindow.h
	noteitem.h
	undo/cmdsetcolour.h
	wizards/nodeeditorwizard.h
	undo/cmdsetpinname.h
	undo/cmdcontextviewpaste.h
	undo/cmdcontextviewcut.h
	undo/cmdmove.h
	wizards/firsttimewizard.h
	performanceform.h
	nodelistform.h
	../stable.h
	snippets/snippetsform.h
	undo/cmdgrouprename.h
	undo/cmdgroup.h
	undo/cmdungroup.h
	undo/cmdgrouppush.h
	undo/cmdgrouppop.h
	undo/cmdpinsplit.h
	undo/cmdpinjoin.h
	../include/fugio/context_widget_signals.h
	../include/fugio/utils.h
	../include/fugio/global.h
	undo/cmdgroupaddoutput.h
	undo/cmdgroupaddinput.h
	undo/cmdgroupreminput.h
	undo/cmdgroupremoutput.h
	patchpromptdialog.h
	model/contextmodel.h
	model/nodemodel.h
	model/basemodel.h
	model/notemodel.h
	model/pinmodel.h
	model/pinlistmodel.h
	model/baselistmodel.h
	settings/settingsdialog.h
	undo/cmdpinmakeglobal.h
	undo/cmdpinremoveglobal.h
	undo/cmdpinconnectglobal.h
	undo/cmdpindisconnectglobal.h
    ../include/fugio/editor_interface.h
    ../include/fugio/editor_signals.h
	)

add_definitions( -DFUGIOLIB_STATIC )

add_executable( Fugio MACOSX_BUNDLE ${SOURCE_FILES} ${UI_RESOURCES} ) 

target_link_libraries( Fugio libfugio Qt5::Core Qt5::Concurrent Qt5::Gui Qt5::Network Qt5::Widgets )

if( APPLE )
	set_target_properties( Fugio PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER com.bigfug.fugio.assimp
		MACOSX_FRAMEWORK_INFO_PLIST Info.plist
	)
endif()

# Add dll paths for MSVC to enable run/debug

if( WIN32 )
#	set( CMAKE_MSVCIDE_RUN_PATH ${LibFoo_RUNTIME_LIBRARY_DIRS} )
endif()

