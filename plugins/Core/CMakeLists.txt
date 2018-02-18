cmake_minimum_required( VERSION 3.1 )

project( fugio-core )

set( SOURCE_FILES
	coreplugin.cpp
    ../../include/fugio/nodecontrolbase.h
    ../../include/fugio/pincontrolbase.h
	../../include/fugio/core/uuid.h
	../../include/fugio/core/variant_interface.h
	../../include/fugio/core/array_interface.h
	../../include/fugio/core/list_interface.h
	../../include/fugio/core/struct_entry_interface.h
	../../include/fugio/core/array_list_interface.h
	stringpin.cpp
    integerpin.cpp
    floatpin.cpp
    triggerpin.cpp
    boolpin.cpp
    bytearraypin.cpp
    numberrangenode.cpp
    randomnumbernode.cpp
    anytriggernode.cpp
    ratecontrolnode.cpp
    numberrangelimitnode.cpp
    boolrangenode.cpp
    togglenode.cpp
    triggerrangenumbernode.cpp
    outputrangenode.cpp
    sizepin.cpp
    splitsizenode.cpp
    joinsizenode.cpp
    arraypin.cpp
    pointpin.cpp
    size3dpin.cpp
    coreplugin.cpp
    splitlistnode.cpp
#    listpin.cpp
    indexnode.cpp
    booltotriggernode.cpp
    signalnumbernode.cpp
    envelopenode.cpp
    numberspreadnode.cpp
    choicepin.cpp
    splitpointnode.cpp
    variantpin.cpp
    duplicatepinsnode.cpp
    lastupdatedinputnode.cpp
    counternode.cpp
    listsizenode.cpp
    listindexnode.cpp
    framedelaynode.cpp
    typesizenode.cpp
    switchnode.cpp
    floatthresholdnode.cpp
    smoothnode.cpp
    autorangenode.cpp
    rectpin.cpp
    getsizenode.cpp
    triggerarraynode.cpp
    triggerbooleannode.cpp
    makearraynode.cpp
    flipflopnode.cpp
    loggernode.cpp
    arraylistentry.h
	bitstoboolnode.cpp
	bitstoboolnode.h
	booltobitsnode.cpp
	booltobitsnode.h
	bitarraypin.cpp
	bitarraypin.h
	linepin.cpp
	linepin.h
	joinpointnode.cpp
	joinpointnode.h
	joinrectnode.cpp
	joinrectnode.h
	transformpin.cpp
	transformpin.h
	polygonpin.cpp
	polygonpin.h
	splitrectnode.cpp
	splitrectnode.h
	integertofloatnode.cpp
	integertofloatnode.h
	blockupdatesnode.cpp
	blockupdatesnode.h
	processclicknode.cpp
	processclicknode.h
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
# Qt5

find_package( Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

#------------------------------------------------------------------------------
# Unit Tests

add_subdirectory( tests )
