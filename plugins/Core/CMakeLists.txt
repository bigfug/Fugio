cmake_minimum_required( VERSION 3.1 )

project( fugio-core )

set( SOURCE_FILES
	coreplugin.cpp
    ../../include/fugio/nodecontrolbase.h
    ../../include/fugio/pincontrolbase.h
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
    listpin.cpp
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
    stringlistpin.cpp
    listsizenode.cpp
    listindexnode.cpp
    framedelaynode.cpp
    typesizenode.cpp
    bytearraylistpin.cpp
    switchnode.cpp
    floatthresholdnode.cpp
    smoothnode.cpp
    autorangenode.cpp
    variantlistpin.cpp
    arraylistpin.cpp
    rectpin.cpp
    getsizenode.cpp
    triggerarraynode.cpp
    triggerbooleannode.cpp
    makearraynode.cpp
    flipflopnode.cpp
    loggernode.cpp
    ../../include/fugio/core/variant_interface.h
    ../../include/fugio/core/array_interface.h
    ../../include/fugio/core/list_interface.h
    ../../include/fugio/core/struct_entry_interface.h
    ../../include/fugio/core/array_list_interface.h
    arraylistpin.h
    arraylistentry.h
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

