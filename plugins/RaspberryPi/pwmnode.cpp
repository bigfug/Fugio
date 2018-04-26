#include "pwmnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include "raspberrypiplugin.h"

PWMNode::PWMNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinAdded(QSharedPointer<fugio::PinInterface>)) );
}

bool PWMNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(frameEnd(qint64)) );

	return( true );
}

bool PWMNode::deinitialise()
{
	disconnect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(frameEnd(qint64)) );

	return( NodeControlBase::deinitialise() );
}

void PWMNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

//	pinUpdated( mPinOutput );
}

QStringList PWMNode::availableInputPins() const
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

bool PWMNode::mustChooseNamedInputPin() const
{
	return( true );
}

void PWMNode::pinAdded( QSharedPointer<fugio::PinInterface> pPin )
{
#if defined( PIGPIO_SUPPORTED )
	const int		PigPio = RaspberryPiPlugin::instance()->piIndex();

	quint16		PId = pPin->name().toInt();
	int			Res = PI_NOT_PERMITTED;

	Res = set_mode( PigPio, PId, PI_OUTPUT );

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
#endif
}

void PWMNode::frameEnd( qint64 pTimestamp )
{
#if defined( PIGPIO_SUPPORTED )
	const int		PigPio = RaspberryPiPlugin::instance()->piIndex();

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		quint16		PId = P->name().toInt();

		if( P->updated() < pTimestamp )
		{
			continue;
		}

		int			Val = variant( P ).toInt();

		set_PWM_dutycycle( PigPio, PId, qBound( 0, Val, 255 ) );
	}
#endif
}
