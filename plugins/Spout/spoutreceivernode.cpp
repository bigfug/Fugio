#include "spoutreceivernode.h"

#include <fugio/core/uuid.h>
#include <fugio/spout/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <fugio/performance.h>

#include "spoutplugin.h"
#include "spoutpin.h"
#include "spoutreceiverform.h"

SpoutReceiverNode::SpoutReceiverNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mOutput = pinOutput<SpoutPin *>( "Texture", mPinOutput, PID_SPOUT );

	mSize = pinOutput<fugio::VariantInterface *>( "Size", mPinSize, PID_SIZE );

	mName = pinOutput<fugio::VariantInterface *>( "Name", mPinName, PID_STRING );

	mPinInputName = pinInput( "Name" );

	mPinInputName->registerPinInputType( PID_STRING );
}

void SpoutReceiverNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp )
	{
		return;
	}

	pinUpdated( mPinOutput );
}

void SpoutReceiverNode::onContextFrame( qint64 pTimeStamp )
{
	fugio::Performance	P( mNode, "contextFrame", pTimeStamp );

	QSize		S;
	QString		N = variant( mPinInputName ).toString();

	if( mOutput->receiveTexture( N ) )
	{
		pinUpdated( mPinOutput );

		QVector3D	T = mOutput->size();

		S = QSize( T[ 0 ], T[ 1 ] );

		N = QString( mOutput->name().data() );
	}

	if( N != mName->variant().toString() )
	{
		mName->setVariant( N );

		pinUpdated( mPinName );
	}

	if( S != mSize->variant().toSize() )
	{
		mSize->setVariant( S );

		pinUpdated( mPinSize );
	}
}

void SpoutReceiverNode::onReceiverName( const QString &pName )
{
	mPinInputName->setValue( pName );
}

bool SpoutReceiverNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if !defined( SPOUT_SUPPORTED )
	if( true )
	{
		mNode->setStatus( fugio::NodeInterface::Error );
		mNode->setStatusMessage( tr( "Spout not supported on this platform" ) );

		return( false );
	}
#endif

	if( !SpoutPlugin::hasOpenGLContextStatic() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameInitialise(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( true );
}

bool SpoutReceiverNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameInitialise(qint64)), this, SLOT(onContextFrame(qint64)) );

	return( NodeControlBase::deinitialise() );
}

QWidget *SpoutReceiverNode::gui()
{
	SpoutReceiverForm		*GUI = new SpoutReceiverForm();

	GUI->setReceiverList( SpoutPlugin::instance()->receiverList() );

	GUI->setReceiverName( mName->variant().toString() );

	connect( GUI, SIGNAL(receiverName(QString)), this, SLOT(onReceiverName(QString)) );

	connect( SpoutPlugin::instance(), SIGNAL(receiverList(QStringList)), GUI, SLOT(setReceiverList(QStringList)) );

	return( GUI );
}
