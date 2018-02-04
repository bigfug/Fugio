#include "mathplugin.h"

#include <QTranslator>
#include <QApplication>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/math/uuid.h>

#include "matrix4pin.h"
#include "vector3pin.h"
#include "vector4pin.h"

#include "addnode.h"
#include "subtractnode.h"
#include "multiplynode.h"
#include "dividenode.h"
#include "modulusnode.h"

#include "absnode.h"

#include "vector3node.h"

#include "splitvector3.h"
#include "joinvector3.h"
#include "splitvector4node.h"
#include "joinvector4node.h"

#include "dotproductnode.h"
#include "crossproducenode.h"
#include "normalisenode.h"

#include "matrixlookatnode.h"
#include "matrixperspectivenode.h"
#include "matrixorthographicnode.h"

#include "matrixrotatenode.h"
#include "matrixtranslatenode.h"
#include "matrixscalenode.h"

#include "matrixmultiplynode.h"
#include "matrixinversenode.h"

#include "sindegreenode.h"
#include "sinradiannode.h"
#include "cosinedegreesnode.h"

#include "pinode.h"

#include "andnode.h"
#include "notnode.h"
#include "ornode.h"
#include "xornode.h"
#include "nandnode.h"
#include "nornode.h"
#include "xnornode.h"

#include "arccosnode.h"

#include "radianstodegreesnode.h"

#include "comparenumbersnode.h"

#include "rotationfromvectorsnode.h"

#include "minmaxnode.h"
#include "minnode.h"
#include "maxnode.h"

#include "ceilnode.h"
#include "floornode.h"
#include "roundnode.h"

#include "quaternionpin.h"

#include "mathexpressionnode.h"

#include "inttobitsnode.h"
#include "multiplexornode.h"
#include "bitstopinsnode.h"

#include "notbitsnode.h"
#include "nandbitsnode.h"
#include "andbitsnode.h"
#include "orbitsnode.h"
#include "xorbitsnode.h"

#include "flipflopnode.h"

#include "pointtransformnode.h"
#include "pownode.h"
#include "numberarraynode.h"
#include "transformnode.h"
#include "sumnode.h"
#include "booleanlatchnode.h"

MathPlugin *MathPlugin::mInstance = nullptr;

QList<QUuid>	NodeControlBase::PID_UUID;

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Abs", "Number", NID_ABS, &AbsNode::staticMetaObject ),
	ClassEntry( "Add", "Math", NID_ADD, &AddNode::staticMetaObject ),
	ClassEntry( "AND", "Logic", NID_AND, &AndNode::staticMetaObject ),
	ClassEntry( "AND Bits", "Logic", NID_AND_BITS, &AndBitsNode::staticMetaObject ),
	ClassEntry( "ArcCos", "Number", NID_ARCCOS, &ArcCosNode::staticMetaObject ),
	ClassEntry( "Bits To Pins", "Math", NID_BITS_TO_PINS, &BitsToPinsNode::staticMetaObject ),
	ClassEntry( "Boolean Latch", "Math", NID_BOOLEAN_LATCH, &BooleanLatchNode::staticMetaObject ),
	ClassEntry( "Compare", "Number", NID_COMPARE_NUMBERS, &CompareNumbersNode::staticMetaObject ),
	ClassEntry( "Cross Product", "Vector3", NID_CROSS_PRODUCT, &CrossProductNode::staticMetaObject ),
	ClassEntry( "Radians to Degrees", "Number", NID_RADIANS_TO_DEGREES, &RadiansToDegreesNode::staticMetaObject ),
	ClassEntry( "Dot Product", "Vector3", NID_DOT_PRODUCT, &DotProductNode::staticMetaObject ),
	ClassEntry( "Ceil", "Math", NID_CEIL, &CeilNode::staticMetaObject ),
	ClassEntry( "Cosine (Degrees)", NID_COSINE_DEGREES, &CosineDegreeNode::staticMetaObject ),
	ClassEntry( "Divide", "Math", NID_DIVIDE, &DivideNode::staticMetaObject ),
	ClassEntry( "Expression", "Math", NID_MATH_EXPRESSION, &MathExpressionNode::staticMetaObject ),
	ClassEntry( "FlipFlop", "Logic", NID_FLIP_FLOP_LOGIC, &FlipFlopNode::staticMetaObject ),
	ClassEntry( "Floor", "Math", NID_FLOOR, &FloorNode::staticMetaObject ),
	ClassEntry( "IntToBits", "Math", NID_INT_TO_BITS, &IntToBitsNode::staticMetaObject ),
	ClassEntry( "Inverse", "Matrix", NID_MATRIX_INVERSE, &MatrixInverseNode::staticMetaObject ),
	ClassEntry( "Join", "Vector3", NID_JOIN_VECTOR3, &JoinVector3Node::staticMetaObject ),
	ClassEntry( "Join", "Vector4", NID_JOIN_VECTOR4, &JoinVector4Node::staticMetaObject ),
	ClassEntry( "Look At", "Matrix", NID_MATRIX_LOOKAT, &MatrixLookAtNode::staticMetaObject ),
	ClassEntry( "Max", "Math", NID_MAX, &MaxNode::staticMetaObject ),
	ClassEntry( "Min", "Math", NID_MIN, &MinNode::staticMetaObject ),
	ClassEntry( "MinMax", "Math", NID_MINMAX, &MinMaxNode::staticMetaObject ),
	ClassEntry( "Modulus", "Math", NID_MODULUS, &ModulusNode::staticMetaObject ),
	ClassEntry( "Multiply", "Math", NID_MULTIPLY, &MultiplyNode::staticMetaObject ),
	ClassEntry( "Multiplexor", "Math", NID_MULTIPLEXOR, &MultiplexorNode::staticMetaObject ),
	ClassEntry( "NAND", "Logic", NID_NAND, &NandNode::staticMetaObject ),
	ClassEntry( "NAND Bits", "Logic", NID_NAND_BITS, &NandBitsNode::staticMetaObject ),
	ClassEntry( "NOT", "Logic", NID_NOT, &NotNode::staticMetaObject ),
	ClassEntry( "NOT Bits", "Logic", NID_NOT_BITS, &NotBitsNode::staticMetaObject ),
	ClassEntry( "NOR", "Logic", NID_NOR, &NorNode::staticMetaObject ),
	ClassEntry( "Normalise", "Vector3", NID_NORMALISE, &NormaliseNode::staticMetaObject ),
	ClassEntry( "Number Array", NID_NUMBER_ARRAY, &NumberArrayNode::staticMetaObject ),
	ClassEntry( "OR", "Logic", NID_OR, &OrNode::staticMetaObject ),
	ClassEntry( "OR Bits", "Logic", NID_OR_BITS, &OrBitsNode::staticMetaObject ),
	ClassEntry( "Orthographic", "Matrix", NID_MATRIX_ORTHOGRAPHIC, &MatrixOrthographicNode::staticMetaObject ),
	ClassEntry( "Perspective", "Matrix", NID_MATRIX_PERSPECTIVE, &MatrixPerspectiveNode::staticMetaObject ),
	ClassEntry( "Pi", "Number", NID_PI, &PiNode::staticMetaObject ),
	ClassEntry( "Point Transform", NID_POINT_TRANSFORM, &PointTransformNode::staticMetaObject ),
	ClassEntry( "Pow", "Math", NID_POW, &PowNode::staticMetaObject ),
	ClassEntry( "Rotate", "Matrix", NID_MATRIX_ROTATE, &MatrixRotateNode::staticMetaObject ),
	ClassEntry( "Rotation From Vectors", "Matrix", NID_ROTATION_FROM_VECTORS, &RotationFromVectorsNode::staticMetaObject ),
	ClassEntry( "Round", "Math", NID_ROUND, &RoundNode::staticMetaObject ),
	ClassEntry( "Scale", "Matrix", NID_MATRIX_SCALE, &MatrixScaleNode::staticMetaObject ),
	ClassEntry( "Sine (Degrees)", NID_SIN_DEGREES, &SinDegreeNode::staticMetaObject ),
	ClassEntry( "Sine (Radians)", NID_SIN_RADIANS, &SinRadianNode::staticMetaObject ),
	ClassEntry( "Split", "Vector3", NID_SPLIT_VECTOR3, &SplitVector3Node::staticMetaObject ),
	ClassEntry( "Split", "Vector4", NID_SPLIT_VECTOR4, &SplitVector4Node::staticMetaObject ),
	ClassEntry( "Subtract", "Math", NID_SUBTRACT, &SubtractNode::staticMetaObject ),
	ClassEntry( "Sum", "Math", NID_SUM, &SumNode::staticMetaObject ),
	ClassEntry( "Transform", "Math", NID_TRANSFORM, &TransformNode::staticMetaObject ),
	ClassEntry( "Translate", "Matrix", NID_MATRIX_TRANSLATE, &MatrixTranslateNode::staticMetaObject ),
	ClassEntry( "Vector3", "GUI", NID_VECTOR3, &Vector3Node::staticMetaObject ),
	ClassEntry( "XNOR", "Logic", NID_XNOR, &XnorNode::staticMetaObject ),
	ClassEntry( "XOR", "Logic", NID_XOR, &XorNode::staticMetaObject ),
	ClassEntry( "XOR Bits", "Logic", NID_XOR_BITS, &XorBitsNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry( "Matrix4", PID_MATRIX4, &Matrix4Pin::staticMetaObject ),
	ClassEntry( "Quaternion", PID_QUATERNION, &QuaternionPin::staticMetaObject ),
	ClassEntry( "Vector3", PID_VECTOR3, &Vector3Pin::staticMetaObject ),
	ClassEntry( "Vector4", PID_VECTOR4, &Vector4Pin::staticMetaObject ),
	ClassEntry()
};

MathPlugin::MathPlugin() : mApp( 0 )
{
	mInstance = this;

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult MathPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerInterface( IID_MATH, this );

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	mApp->registerPinSplitter( PID_VECTOR3, NID_SPLIT_VECTOR3 );
	mApp->registerPinSplitter( PID_VECTOR4, NID_SPLIT_VECTOR4 );

	mApp->registerPinJoiner( PID_VECTOR3, NID_JOIN_VECTOR3 );
	mApp->registerPinJoiner( PID_VECTOR4, NID_JOIN_VECTOR4 );

	mApp->registerPinForMetaType( PID_VECTOR3, QMetaType::QVector3D );
	mApp->registerPinForMetaType( PID_VECTOR4, QMetaType::QVector4D );

	return( INIT_OK );
}

void MathPlugin::deinitialise( void )
{
	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp->unregisterInterface( IID_MATH );

	mApp = 0;
}

void MathPlugin::registerMetaTypeMathOperator(QMetaType::Type pType, MathInterface::MathOperator pOperator, MathInterface::MathOperatorFunction pFunction)
{
	mMathFunc.insert( MathFuncPair( pType, pOperator ), pFunction );
}

void MathPlugin::deregisterMetaTypeMathOperator(QMetaType::Type pType, MathInterface::MathOperator pOperator )
{
	mMathFunc.remove( MathFuncPair( pType, pOperator ) );
}

MathInterface::MathOperatorFunction MathPlugin::findMetaTypeMathOperator(QMetaType::Type pType, MathInterface::MathOperator pOperator) const
{
	return( mMathFunc.value( MathFuncPair( pType, pOperator ), nullptr ) );
}
