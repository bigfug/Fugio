#include "matrixinversenode.h"

#include <QMatrix4x4>
#include <QVector4D>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/context_interface.h>

MatrixInverseNode::MatrixInverseNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_MATRIX, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_MATRIX, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinMatrixInput = pinInput( "Matrix", PIN_INPUT_MATRIX );

	mPinMatrixInput->registerPinInputType( PID_MATRIX4 );

	mValMatrixOutput = pinOutput<fugio::VariantInterface *>( "Matrix", mPinMatrixOutput, PID_MATRIX4, PIN_OUTPUT_MATRIX );
}

void MatrixInverseNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	bool			IOK;
	QMatrix4x4		Mat = variant( mPinMatrixInput ).value<QMatrix4x4>();
	QMatrix4x4		Inv = Mat.inverted( &IOK );

	if( IOK && Inv != mValMatrixOutput->variant().value<QMatrix4x4>() )
	{
		mValMatrixOutput->setVariant( Inv );

		pinUpdated( mPinMatrixOutput );
	}
}
