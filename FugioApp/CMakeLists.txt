cmake_minimum_required( VERSION 2.8.11 )

project( FugioApp )

find_package( Qt5 COMPONENTS Core Concurrent Gui Network Widgets REQUIRED ) 
 
set( UI_RESOURCES fugio.qrc )

include_directories( ../FugioLib )

set( SOURCE_FILES
	../include/fugio/context_widget_interface.h
	../include/fugio/context_widget_signals.h
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
    ../include/fugio/editor_interface.h
    ../include/fugio/editor_signals.h
	)

set( UIS_HDRS
	mainwindow.ui
	snippets/snippetsform.ui
	settings/settingsdialog.ui
	contextwidgetprivate.ui
	stylesheetform.ui
	performanceform.ui
	patchpromptdialog.ui
	nodenamedialog.ui
	nodelistform.ui
)

add_definitions( -DFUGIOLIB_STATIC )

add_executable( Fugio MACOSX_BUNDLE ${SOURCE_FILES} ${UI_RESOURCES} ${UIS_HDRS} ) 

if( APPLE )
	set_target_properties( Fugio PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER com.bigfug.fugio
		MACOSX_FRAMEWORK_INFO_PLIST Info.plist
	)
endif()
