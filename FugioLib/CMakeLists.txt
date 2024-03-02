cmake_minimum_required( VERSION 3.14 )

project( libfugio )

set( UI_RESOURCES resources.qrc )

set( SOURCE_FILES
	fugio.cpp
	contextprivate.cpp
	contextprivate.h
	context.cpp
	globalprivate.cpp
	globalprivate.h
	global.cpp
	pinprivate.cpp
	pinprivate.h
	pin.cpp
	pluginmanager.h
	pluginmanager.cpp
	nodeprivate.cpp
	nodeprivate.h
	node.cpp
	interpolation.cpp
	universe.cpp
	universe.h

	../include/fugio/global.h
	../libs/FugioTime/lib/timesync.cpp
	../libs/FugioTime/lib/timesync.h
	../include/fugio/interpolation.h
	../include/fugio/performance.h
	../stable.h
	../include/fugio/pluginbase.h
	../include/fugio/choice_interface.h
	../include/fugio/global_interface.h
	../include/fugio/plugin_interface.h
	../include/fugio/serialise_interface.h
	../include/fugio/pin_signals.h
	../include/fugio/node_signals.h
	../include/fugio/playhead_interface.h
	../include/fugio/pin_control_interface.h
	../include/fugio/pin_interface.h
	../include/fugio/node_control_interface.h
	../include/fugio/node_interface.h
	../include/fugio/edit_interface.h
	../include/fugio/device_factory_interface.h
	../include/fugio/context_widget_interface.h
	../include/fugio/context_interface.h
	../include/fugio/global_signals.h
	../include/fugio/utils.h
	../include/fugio/context_signals.h
	../include/fugio/paired_pins_helper_interface.h
	../include/fugio/device_provider_interface.h

	../libs/zip/zip.h
	../libs/zip/zip.cpp
	../libs/zip/miniz.h
)

add_library( ${PROJECT_NAME} STATIC ${SOURCE_FILES} )

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	FUGIOLIB_STATIC
#	GLOBAL_THREADED
	)

#------------------------------------------------------------------------------
# Qt

find_package(Qt6 COMPONENTS Core Concurrent Network Widgets LinguistTools QUIET)

if (NOT Qt6_FOUND)
	find_package(Qt5 5.15 COMPONENTS Core Concurrent Network Widgets LinguistTools QUIET)
endif()

target_link_libraries( ${PROJECT_NAME} PRIVATE Qt::Core Qt::Network Qt::Widgets )

set( TS_FILES
	translations/translations_de.ts
	translations/translations_es.ts
	translations/translations_fr.ts
	translations/translations_it.ts
	)

if( COMMAND qt_add_translations )
	qt_add_translations( ${PROJECT_NAME} TS_FILES ${TS_FILES} RESOURCE_PREFIX "/translations" )
else()
	qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

	configure_file( ${CMAKE_SOURCE_DIR}/translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY )

	target_sources( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc ${QM_FILES} )
endif()

#------------------------------------------------------------------------------
# Unit Testing

add_subdirectory( tests )
