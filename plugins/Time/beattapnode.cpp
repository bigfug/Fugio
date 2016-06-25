#include "beattapnode.h"

#include <cmath>

#include <QDateTime>
#include <QDebug>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>
#include <fugio/core/variant_interface.h>

BeatTapNode::BeatTapNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mBeatDuration( 1000.0 ), mBeatStart( -1 ), mBeatSync( 0 )
{
	mPinTrigger = pinInput( "Trigger" );

	mValBPM = pinOutput<fugio::VariantInterface *>( "BPM", mPinBPM, PID_FLOAT );

	mValSync = pinOutput<fugio::VariantInterface *>( "Sync", mPinSync, PID_FLOAT );

	pinOutput<fugio::PinControlInterface *>( "Beat", mPinBeat, PID_TRIGGER );
}

bool BeatTapNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	mValBPM->setVariant( 60000.0 / mBeatDuration );

	mNode->context()->pinUpdated( mPinBPM );

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	return( true );
}

void BeatTapNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !mBeatTime.isEmpty() && pTimeStamp - mBeatTime.last() >= 2000 )
	{
		mBeatTime.clear();
	}

	mBeatLast = pTimeStamp;

	mNode->context()->pinUpdated( mPinBeat );

	mBeatTime.append( pTimeStamp );

	if( mBeatTime.size() < 2 )
	{
		return;
	}

	while( mBeatTime.size() > 8 )
	{
		mBeatTime.removeFirst();
	}

	mBeatDuration = double( mBeatTime.last() - mBeatTime.first() ) / double( mBeatTime.size() - 1 );

	mValBPM->setVariant( 60000.0 / mBeatDuration );

	mNode->context()->pinUpdated( mPinBPM );
}

void BeatTapNode::frameStart( qint64 pTimeStamp )
{
	if( mBeatDuration <= 0 )
	{
		return;
	}

	if( mBeatStart < 0 )
	{
		mBeatStart = pTimeStamp;
		mBeatLast  = pTimeStamp;
		mBeatSync  = 0;
	}

	double		BeatDiff = double( pTimeStamp ) - mBeatLast;

	if( BeatDiff > mBeatDuration )
	{
		mNode->context()->pinUpdated( mPinBeat );
	}

	while( BeatDiff > mBeatDuration )
	{
		BeatDiff -= mBeatDuration;

		mBeatLast += mBeatDuration;
		mBeatSync += 1.0;
	}

	mBeatSync = std::floor( mBeatSync ) + ( BeatDiff / mBeatDuration );

	mValSync->setVariant( mBeatSync );

	mNode->context()->pinUpdated( mPinSync );
}

void BeatTapNode::loadSettings( QSettings &pSettings )
{
	mBeatDuration = pSettings.value( "duration", mBeatDuration ).toDouble();
}

void BeatTapNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "duration", mBeatDuration );
}
