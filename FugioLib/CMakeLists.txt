cmake_minimum_required( VERSION 3.1 )

project( libfugio )

set( UI_RESOURCES resources.qrc )

set( SOURCE_FILES
	fugio.cpp
	contextprivate.cpp
	context.cpp
	globalprivate.cpp
	global.cpp
	pinprivate.cpp
	pin.cpp
	nodeprivate.cpp
	node.cpp
	interpolation.cpp
	../include/fugio/global.h
	contextprivate.h
	globalprivate.h
	pinprivate.h
	nodeprivate.h
	../libs/FugioTime/lib/timesync.cpp
	../libs/FugioTime/lib/timesync.h
	universe.cpp
	universe.h
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
)

add_library( ${PROJECT_NAME} STATIC ${SOURCE_FILES} )

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	FUGIOLIB_STATIC
#	GLOBAL_THREADED
	)

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 REQUIRED COMPONENTS Core Concurrent Network Widgets LinguistTools )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Unit Testing

add_subdirectory( tests )
