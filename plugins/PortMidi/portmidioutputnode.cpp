#include "portmidioutputnode.h"

#include "devicemidi.h"

#include <QComboBox>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/midi/uuid.h>
#include <fugio/midi/midi_interface.h>
#include <fugio/core/array_interface.h>

#include <fugio/context_signals.h>

#include <fugio/core/uuid.h>

#include <fugio/utils.h>

#include <fugio/node_signals.h>

PortMidiOutputNode::PortMidiOutputNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDevice( 0 ), mClockValue( 0 ), mClockLast( 0 ), mPlayState( false )
{
	mPinClock = pinInput( "Beat Position" );

	mPinMidi  = pinInput( "MIDI" );

	mPinMidi->registerPinInputType( PID_MIDI_OUTPUT );
}

bool PortMidiOutputNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameProcess()), this, SLOT(onFrameStart()) );
	connect( mNode->context()->qobject(), SIGNAL(frameEnd(qint64)), this, SLOT(onFrameEnd(qint64)) );

	return( true );
}

bool PortMidiOutputNode::deinitialise()
{
	mNode->context()->qobject()->disconnect( this );

	return( NodeControlBase::deinitialise() );
}

void PortMidiOutputNode::onFrameStart()
{
	if( !mDevice )
	{
		return;
	}

	PmEvent	E;

	E.timestamp = 0;

	if( mNode->context()->isPlaying() )
	{
		if( !mPlayState )
		{
			if( mNode->context()->position() == 0.0 )
			{
				E.message = Pm_Message( MIDI_START, 0, 0 );
			}
			else
			{
				E.message = Pm_Message( MIDI_CONTINUE, 0, 0 );
			}

			mDevice->output( E );
		}
	}
	else
	{
		if( mPlayState )
		{
			PmEvent	E;

			E.message   = Pm_Message( MIDI_STOP, 0, 0 );
			E.timestamp = 0;

			mDevice->output( E );
		}
	}

	mPlayState = mNode->context()->isPlaying();
}

void PortMidiOutputNode::onFrameEnd( qint64 pTimeStamp )
{
	if( !mDevice )
	{
		return;
	}

	if( mPinClock->isConnected() && mPinClock->isUpdated( pTimeStamp ) )
	{
		fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( mPinClock->connectedPin()->control()->qobject() );

		if( V )
		{
			PmEvent	E;
			qreal	C = V->variant().toDouble();
			qreal	I = 1./24.;

			E.message   = Pm_Message( MIDI_CLOCK, 0, 0 );
			E.timestamp = 0;

			if( !mNode->context()->isPlaying() )
			{
				mClockValue = C / I;
			}
			else
			{
				if( std::abs( ( mClockValue * I ) - C ) > 0.25 )
				{
					mClockValue = C / I;
				}

				while( C > mClockValue * I )
				{
					mDevice->output( E );

					mClockValue += 1;
				}
			}
		}
	}

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P == mPinClock )
		{
			continue;
		}

		fugio::MidiInterface		*M = input<fugio::MidiInterface *>( P );

		if( M )
		{
			mDevice->output( M->messages() );

			M->clearData();

			continue;
		}

		fugio::ArrayInterface		*A = input<fugio::ArrayInterface *>( P );

		if( A )
		{
			if( A->type() == QMetaType::Int && A->size() == 1 && A->stride() == sizeof( int ) )
			{
				mDevice->output( (const int32_t *)A->array(), A->count() );
			}

			continue;
		}

		fugio::VariantInterface		*V = input<fugio::VariantInterface *>( P );

		if( V )
		{
			if( P->controlUuid() == PID_BYTEARRAY )
			{
				mDevice->outputSysEx( V->variant().toByteArray() );
			}
			else if( P->controlUuid() == PID_BYTEARRAY_LIST )
			{
				for( const QVariant &L : V->variant().toList() )
				{
					const QByteArray	&BA = L.toByteArray();

					if( !BA.isEmpty() )
					{
						mDevice->outputSysEx( BA );
					}
				}
			}
		}
	}
}

void PortMidiOutputNode::setDeviceName( const QString &pDeviceName )
{
	if( !mDevice || pDeviceName != mDeviceName )
	{
		mDevice = DeviceMidi::findDeviceOutput( pDeviceName );
	}

	mDeviceName = pDeviceName;

	mNode->setStatus( mDevice && mDevice->isActive() ? fugio::NodeInterface::Initialised : fugio::NodeInterface::Warning );
}

QWidget *PortMidiOutputNode::gui()
{
	QComboBox							*GUI;

	if( ( GUI = new QComboBox() ) != nullptr )
	{
		QStringList			DevLst = DeviceMidi::deviceOutputNames();

		if( !mDeviceName.isEmpty() && !DevLst.contains( mDeviceName ) )
		{
			DevLst << mDeviceName;

			std::sort( DevLst.begin(), DevLst.end() );
		}

		DevLst.prepend( tr( "<None>" ) );

		for( int i = 0 ; i < DevLst.size() ; i++ )
		{
			GUI->addItem( DevLst[ i ] );

			if( !mDeviceName.isEmpty() && mDeviceName == DevLst[ i ] )
			{
				GUI->setCurrentIndex( i );
			}
		}

		connect( GUI, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDeviceName(QString)) );

		connect( this, SIGNAL(deviceChanged(QString)), GUI, SLOT(setCurrentText(QString)) );
	}

	return( GUI );
}

void PortMidiOutputNode::loadSettings( QSettings &pSettings )
{
	QString		NewDevNam = pSettings.value( "device-name", mDeviceName ).toString();

	if( NewDevNam != mDeviceName )
	{
		mDeviceName = NewDevNam;

		if( !mDevice || mDevice->name() != mDeviceName )
		{
			mDevice = DeviceMidi::findDeviceOutput( mDeviceName );
		}

		emit deviceChanged( mDeviceName );
	}

	mNode->setStatus( mDevice && mDevice->isActive() ? fugio::NodeInterface::Initialised : fugio::NodeInterface::Warning );
}

void PortMidiOutputNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "device-name", mDeviceName );
}

QList<QUuid> PortMidiOutputNode::pinAddTypesInput() const
{
	static QList<QUuid>	PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_MIDI_OUTPUT;
		PinLst << PID_BYTEARRAY_LIST;
	}

	return( PinLst );
}

bool PortMidiOutputNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
