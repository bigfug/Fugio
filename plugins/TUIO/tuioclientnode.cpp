#include "tuioclientnode.h"

#include <QJsonDocument>
#include <QJsonArray>

#include <fugio/core/uuid.h>
#include <fugio/tuio/uuid.h>
#include <fugio/json/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/pin_variant_iterator.h>
#include <fugio/performance.h>

TuioClientNode::TuioClientNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mClient( Q_NULLPTR )
{
	FUGID( PIN_INPUT_DATA, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_DATA, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInput = pinInput( "Input", PIN_INPUT_DATA );

	mValOutputData = pinOutput<fugio::VariantInterface *>( "Cursor", mPinOutputData, PID_JSON, PIN_OUTPUT_DATA );
}

bool TuioClientNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	mClient = new TUIO::TuioClient( this );

	if( !mClient )
	{
		return( false );
	}

	mClient->addTuioListener( this );

	return( true );
}

bool TuioClientNode::deinitialise()
{
	if( mClient )
	{
		mClient->removeTuioListener( this );

		delete mClient;

		mClient = Q_NULLPTR;
	}

	return( NodeControlBase::deinitialise() );
}

void TuioClientNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance		Performance( mNode, "inputsUpdated", pTimeStamp );

	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		fugio::PinVariantIterator	Packets( mPinInput );

		mValOutputData->variantClear();

		for( int i = 0 ; i < Packets.count() ; i++ )
		{
			QByteArray	A = Packets.index( i ).toByteArray();

			if( !A.isEmpty() )
			{
				ProcessPacket( A.constData(), A.size(), IpEndpointName() );
			}
		}

		if( mValOutputData->variantCount() )
		{
			pinUpdated( mPinOutputData );
		}
	}
}

void TuioClientNode::addTuioObject(TUIO::TuioObject *tobj)
{
	reportObject( tobj, "add" );
}

void TuioClientNode::updateTuioObject(TUIO::TuioObject *tobj)
{
	reportObject( tobj, "update" );
}

void TuioClientNode::removeTuioObject(TUIO::TuioObject *tobj)
{
	reportObject( tobj, "remove" );
}

void TuioClientNode::addTuioCursor( TUIO::TuioCursor *tcur )
{
	reportCursor( tcur, "add" );
}

void TuioClientNode::updateTuioCursor(TUIO::TuioCursor *tcur)
{
	reportCursor( tcur, "update" );
}

void TuioClientNode::removeTuioCursor(TUIO::TuioCursor *tcur)
{
	reportCursor( tcur, "remove" );
}

void TuioClientNode::addTuioBlob(TUIO::TuioBlob *tblb)
{
	reportBlob( tblb, "add" );
}

void TuioClientNode::updateTuioBlob(TUIO::TuioBlob *tblb)
{
	reportBlob( tblb, "update" );
}

void TuioClientNode::removeTuioBlob(TUIO::TuioBlob *tblb)
{
	reportBlob( tblb, "remove" );
}

void TuioClientNode::refresh( TUIO::TuioTime ftime )
{
//	qDebug() << "refresh" << ftime.getTotalMilliseconds();
}

void TuioClientNode::reportBlob(TUIO::TuioBlob *tblb, const QString &pType)
{
	QJsonObject		Object;

	Object.insert( "blob_id", tblb->getBlobID() );
	Object.insert( "type", pType );

	QJsonObject		Position;

	Position.insert( "x", tblb->getX() );
	Position.insert( "y", tblb->getY() );

	Object.insert( "position", Position );

	QJsonObject		Speed;

	Speed.insert( "x", tblb->getXSpeed() );
	Speed.insert( "y", tblb->getYSpeed() );

	Object.insert( "speed", Speed );

	mValOutputData->variantAppend( QJsonDocument( Object ) );
}

void TuioClientNode::reportCursor(TUIO::TuioCursor *tcur, const QString &pType)
{
	if( tcur->getCursorID() == 0 )
	{
		qDebug() << tcur->getX() << tcur->getY();
	}

	QJsonObject		Object;

	Object.insert( "cursor_id", tcur->getCursorID() );
	Object.insert( "type", pType );

	QJsonObject		Position;

	Position.insert( "x", tcur->getX() );
	Position.insert( "y", tcur->getY() );

	Object.insert( "position", Position );

	QJsonObject		Speed;

	Speed.insert( "x", tcur->getXSpeed() );
	Speed.insert( "y", tcur->getYSpeed() );

	Object.insert( "speed", Speed );

	mValOutputData->variantAppend( QJsonDocument( Object ) );
}

void TuioClientNode::reportObject(TUIO::TuioObject *tobj, const QString &pType)
{
	QJsonObject		Object;

	Object.insert( "object_id", tobj->getSymbolID() );
	Object.insert( "type", pType );

	QJsonObject		Position;

	Position.insert( "x", tobj->getX() );
	Position.insert( "y", tobj->getY() );

	Object.insert( "position", Position );

	QJsonObject		Speed;

	Speed.insert( "x", tobj->getXSpeed() );
	Speed.insert( "y", tobj->getYSpeed() );

	Object.insert( "speed", Speed );

	mValOutputData->variantAppend( QJsonDocument( Object ) );
}

void TuioClientNode::connect(bool lock)
{
	connected = true;
}

void TuioClientNode::disconnect()
{
	connected = false;
}
