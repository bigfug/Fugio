#include "gpionode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include "raspberrypiplugin.h"

GPIONode::GPIONode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinAdded(QSharedPointer<fugio::PinInterface>)) );
}

bool GPIONode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(frameStart()) );
	connect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(frameEnd(qint64)) );

	return( true );
}

bool GPIONode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(frameEnd(qint64)) );
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(frameStart()) );

	return( NodeControlBase::deinitialise() );
}

void GPIONode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

//	pinUpdated( mPinOutput );
}

QStringList GPIONode::availableInputPins() const
{
	QStringList		PinLst;

	for( unsigned p = 0 ; p <= 53 ; p++ )
	{
		QString	Name = QString::number( p );

		if( !mNode->findPinByName( Name ) )
		{
			PinLst << Name;
		}
	}

	return( PinLst );
}

bool GPIONode::mustChooseNamedInputPin() const
{
	return( true );
}

void GPIONode::pinAdded( QSharedPointer<fugio::PinInterface> pPin )
{
	const int		PigPio = RaspberryPiPlugin::instance()->piIndex();

	quint16		PId = pPin->name().toInt();
	int			Res = PI_NOT_PERMITTED;

	if( pPin->direction() == PIN_OUTPUT )
	{
		Res = set_mode( PigPio, PId, PI_INPUT );

	//	Res = set_pull_up_down( PigPio, PId, PI_PUD_DOWN );
	}
	else
	{
		Res = set_mode( PigPio, PId, PI_OUTPUT );
	}

	if( Res == PI_BAD_GPIO )
	{
		qWarning() << "PI_BAD_GPIO";
	}
	else if( Res == PI_BAD_MODE )
	{
		qWarning() << "PI_BAD_MODE";
	}
	else if( Res == PI_NOT_PERMITTED )
	{
		qWarning() << "PI_NOT_PERMITTED";
	}
}

void GPIONode::frameStart()
{
	const int		PigPio = RaspberryPiPlugin::instance()->piIndex();

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		quint16		PId = P->name().toInt();

		int			Res = gpio_read( PigPio, PId );

		if( Res != PI_BAD_GPIO )
		{
			fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() );

			if( V && V->variant().toInt() != Res )
			{
				V->setVariant( Res );

				pinUpdated( P );
			}
		}
	}
}

void GPIONode::frameEnd( qint64 pTimestamp )
{
	const int		PigPio = RaspberryPiPlugin::instance()->piIndex();

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		quint16		PId = P->name().toInt();

		if( P->updated() < pTimestamp )
		{
			continue;
		}

		int			Val = variant( P ).toInt();
		int			Res = gpio_write( PigPio, PId, Val );
	}
}


QList<fugio::NodeControlInterface::AvailablePinEntry> GPIONode::availableOutputPins() const
{
	QList<fugio::NodeControlInterface::AvailablePinEntry>	PinLst;
//	const int		PigPio = RaspberryPiPlugin::instance()->piIndex();

	for( unsigned p = 0 ; p <= 53 ; p++ )
	{
		fugio::NodeControlInterface::AvailablePinEntry	PE( QString::number( p ), PID_INTEGER );

		if( !mNode->findPinByName( PE.mName ) )
		{
			PinLst << PE;
		}
	}

	return( PinLst );
}

bool GPIONode::mustChooseNamedOutputPin() const
{
	return( true );
}
