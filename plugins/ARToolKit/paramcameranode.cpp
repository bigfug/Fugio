#include "paramcameranode.h"

#include <QMatrix4x4>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/artoolkit/uuid.h>

#if defined( ARTOOLKIT_SUPPORTED )
#include <AR/ar.h>
#endif

ParamCameraNode::ParamCameraNode( QSharedPointer<fugio::NodeInterface> pNode )
	:NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_PARAM, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_SIZE, "fe19f444-7a5f-47e6-ae83-ceb43b8ae915" );
	FUGID( PIN_INPUT_NEAR, "1ceaeb2d-0463-44bc-9b80-10129cd8f4eb" );
	FUGID( PIN_INPUT_FAR, "5064e449-8b0b-4447-9009-c81997f754ef" );
	FUGID( PIN_OUTPUT_MATRIX, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputParam = pinInput( "Param", PIN_INPUT_PARAM );

	mPinInputParam->registerPinInputType( PID_AR_PARAM );

	mPinInputSize = pinInput( "Size", PIN_INPUT_SIZE );

	mPinInputSize->registerPinInputType( PID_SIZE );

	mPinInputNear = pinInput( "Near", PIN_INPUT_NEAR );
	mPinInputFar  = pinInput( "Far", PIN_INPUT_FAR );

	mPinInputNear->setValue( 5.0f );
	mPinInputFar->setValue( 2000.0f );

	mValOutputMatrix = pinOutput<fugio::VariantInterface *>( "Matrix", mPinOutputMatrix, PID_MATRIX4, PIN_OUTPUT_MATRIX );
}

void ParamCameraNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ar::ParamInterface		*PrmInt = input<fugio::ar::ParamInterface *>( mPinInputParam );

	if( !PrmInt )
	{
		return;
	}

#if defined( ARTOOLKIT_SUPPORTED )
	ARParam			PrmDat = PrmInt->param();

	if( !mPinInputSize->isUpdated( pTimeStamp ) || !mPinInputNear->isUpdated( pTimeStamp ) || !mPinInputFar->isUpdated( pTimeStamp ) )
	{
		return;
	}

	QSize			CamSze = variant( mPinInputSize ).toSize();

	arParamChangeSize( &PrmDat, CamSze.width(), CamSze.height(), &PrmDat );

	QMatrix4x4		CamMat;

	arglCameraFrustumRHf( &PrmDat, variant( mPinInputNear ).toFloat(), variant( mPinInputFar ).toFloat(), CamMat.data() );

	mValOutputMatrix->setVariant( CamMat );

	pinUpdated( mPinOutputMatrix );
#endif
}
