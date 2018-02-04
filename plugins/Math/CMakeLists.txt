cmake_minimum_required( VERSION 3.1 )

project( fugio-math )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/math/matrix_interface.h
	../../include/fugio/math/uuid.h

	absnode.cpp
	absnode.h
	addnode.cpp
	addnode.h
	andnode.cpp
	andnode.h
	arccosnode.cpp
	arccosnode.h
	booleanlatchnode.cpp
	booleanlatchnode.h
	ceilnode.cpp
	ceilnode.h
	comparenumbersnode.cpp
	comparenumbersnode.h
	crossproducenode.cpp
	crossproducenode.h
	dividenode.cpp
	dividenode.h
	dotproductnode.cpp
	dotproductnode.h
	floornode.cpp
	floornode.h
	joinvector3.cpp
	joinvector3.h
	joinvector4node.cpp
	joinvector4node.h
	mathplugin.cpp
	mathplugin.h
	matrix4node.cpp
	matrix4node.h
	matrix4pin.cpp
	matrix4pin.h
	matrixinversenode.cpp
	matrixinversenode.h
	matrixlookatnode.cpp
	matrixlookatnode.h
	matrixmultiplynode.cpp
	matrixmultiplynode.h
	matrixorthographicnode.cpp
	matrixorthographicnode.h
	matrixperspectivenode.cpp
	matrixperspectivenode.h
	matrixrotatenode.cpp
	matrixrotatenode.h
	matrixscalenode.cpp
	matrixscalenode.h
	matrixtranslatenode.cpp
	matrixtranslatenode.h
	minmaxnode.cpp
	minmaxnode.h
	modulusnode.cpp
	modulusnode.h
	multiplynode.cpp
	multiplynode.h
	nandnode.cpp
	nandnode.h
	normalisenode.cpp
	normalisenode.h
	nornode.cpp
	nornode.h
	notnode.cpp
	notnode.h
	ornode.cpp
	ornode.h
	pinode.cpp
	pinode.h
	quaternionpin.cpp
	quaternionpin.h
	radianstodegreesnode.cpp
	radianstodegreesnode.h
	rotationfromvectorsnode.cpp
	rotationfromvectorsnode.h
	roundnode.cpp
	roundnode.h
	sindegreenode.cpp
	sindegreenode.h
	sinradiannode.cpp
	sinradiannode.h
	splitvector3.cpp
	splitvector3.h
	splitvector4node.cpp
	splitvector4node.h
	subtractnode.cpp
	subtractnode.h
	vector3node.cpp
	vector3node.h
	vector3pin.cpp
	vector3pin.h
	vector4pin.cpp
	vector4pin.h
	xnornode.cpp
	xnornode.h
	xornode.cpp
	xornode.h
    mathexpressionnode.cpp
    mathexpressionnode.h

	orbitsnode.cpp
	orbitsnode.h
	andbitsnode.cpp
	andbitsnode.h
	notbitsnode.cpp
	notbitsnode.h
	xorbitsnode.cpp
	xorbitsnode.h
	flipflopnode.cpp
	flipflopnode.h
	nandbitsnode.cpp
	nandbitsnode.h
	inttobitsnode.cpp
	inttobitsnode.h
	transformnode.cpp
	transformnode.h
	bitstopinsnode.cpp
	bitstopinsnode.h
	multiplexornode.cpp
	multiplexornode.h
	numberarraynode.cpp
	numberarraynode.h
	cosinedegreesnode.cpp
	cosinedegreesnode.h
	pointtransformnode.cpp
	pointtransformnode.h
	maxnode.cpp
	maxnode.h
	minnode.cpp
	minnode.h
	pownode.cpp
	pownode.h
	sumnode.cpp
	sumnode.h
)

set( QRC_FILES
#	resources.qrc
)

if( MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
endif( MSVC )

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )
