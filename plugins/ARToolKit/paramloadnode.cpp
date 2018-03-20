#include "paramloadnode.h"

#include <QMatrix4x4>
#include <QFile>

#include <fugio/artoolkit/uuid.h>

#if defined( ARTOOLKIT_SUPPORTED )
#include <AR/ar.h>
#endif

ParamLoadNode::ParamLoadNode( QSharedPointer<fugio::NodeInterface> pNode )
	:NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FILENAME, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_PARAM, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputFilename = pinInput( "Filename", PIN_INPUT_FILENAME );

	mValOutputParam = pinOutput<fugio::ar::ParamInterface *>( "Param", mPinOutputParam, PID_AR_PARAM, PIN_OUTPUT_PARAM );
}

void ParamLoadNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QString			FN = variant( mPinInputFilename ).toString();

	if( FN.isEmpty() || !QFile::exists( FN ) )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

#if defined( ARTOOLKIT_SUPPORTED )
	ARParam			PR;

	if( arParamLoad( FN.toLatin1().constData(), 1, &PR ) < 0 )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	if( memcmp( &PR, &mValOutputParam->param(), sizeof( ARParam ) ) != 0 )
	{
		mValOutputParam->setParam( PR );

		pinUpdated( mPinOutputParam );
	}
#endif
}
