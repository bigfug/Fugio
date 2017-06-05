cmake_minimum_required( VERSION 3.1 )

project( fugio-time )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	
	../../include/fugio/time/uuid.h

	beattapnode.cpp
	beattapnode.h
	clocknode.cpp
	clocknode.h
	cronnode.cpp
	cronnode.h
	datenode.cpp
	datenode.h
	delaynode.cpp
	delaynode.h
	everynode.cpp
	everynode.h
	inertianode.cpp
	inertianode.h
	playheadcontrolnode.cpp
	playheadcontrolnode.h
	playheadframesnode.cpp
	playheadframesnode.h
	playheadnode.cpp
	playheadnode.h
	timenode.cpp
	timenode.h
	timeplugin.cpp
	timeplugin.h
	
	cronform.ui
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

