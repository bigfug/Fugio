#include "lednode.h"

#include <cmath>

#include <QWidget>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/context_signals.h>

#include "leditem.h"

LedNode::LedNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLedVal( 0 ), mLastTime( -1 ), mLedColour( Qt::red ),
	  mLedMode( BOOLEAN )
{
	mPinInput = pinInput( "Input" );
}

bool LedNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onFrameStart(qint64)) );

	return( true );
}

void LedNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInput->isUpdated( pTimeStamp ) )
	{
		fugio::VariantInterface		*V = input<fugio::VariantInterface *>( mPinInput );

		if( V )
		{
			switch( QMetaType::Type( V->variant().type() ) )
			{
				case QMetaType::Bool:
					{
						mLedVal = V->variant().toBool() ? 1.0 : 0.0;

						mLastTime = pTimeStamp;

						mLedMode = BOOLEAN;
					}
					break;

				case QMetaType::Float:
				case QMetaType::Double:
					{
						bool	ValChk;
						double	NewVal = V->variant().toDouble( &ValChk );

						if( ValChk )
						{
							NewVal = qBound( 0.0, NewVal, 1.0 );

							mLedTgt = NewVal;

							mLastTime = pTimeStamp;

							mLedMode = INTERPOLATE;
						}
					}
					break;

				default:
					{
						mLedVal = 1.0;

						mLastTime = pTimeStamp;

						mLedMode = FADE;
					}
					break;
			}
		}
		else
		{
			mLedVal = 1.0;

			mLastTime = pTimeStamp;

			mLedMode = FADE;
		}

		emit brightnessChanged( mLedVal );
	}
}

void LedNode::onFrameStart( qint64 pTimeStamp )
{
	if( mLastTime < 0 )
	{
		mLastTime = pTimeStamp;
	}

	if( mLedMode == FADE && mLedVal > 0.0 )
	{
		double	TimeDiff  = double( pTimeStamp ) - double( mLastTime );
		double	ValueDrop = ( TimeDiff / 500.0 );

		mLedVal = qBound( 0.0, mLedVal - ValueDrop, 1.0 );

		emit brightnessChanged( mLedVal );
	}

	if( mLedMode == INTERPOLATE && mLedVal != mLedTgt )
	{
		double	TimeDiff  = ( double( pTimeStamp ) - double( mLastTime ) ) / 500.0;

		if( std::abs( mLedVal - mLedTgt ) < TimeDiff )
		{
			mLedVal = mLedTgt;
		}
		else if( mLedVal < mLedTgt )
		{
			mLedVal = mLedTgt;
		}
		else
		{
			mLedVal -= TimeDiff;
		}

		emit brightnessChanged( mLedVal );
	}

	mLastTime = pTimeStamp;
}

void LedNode::onColourChanged( const QColor &pColour )
{
	mLedColour = pColour;
}

void LedNode::loadSettings( QSettings &pSettings )
{
	mLedColour = pSettings.value( "colour", mLedColour ).value<QColor>();

	emit colourChanged( mLedColour );
}

void LedNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "colour", mLedColour );
}

QGraphicsItem *LedNode::guiItem()
{
	LEDItem			*GUI = new LEDItem();

	if( GUI )
	{
		GUI->setColour( mLedColour );

		GUI->setBrightness( mLedVal );

		connect( this, SIGNAL(brightnessChanged(double)), GUI, SLOT(setBrightness(double)) );

		connect( this, SIGNAL(colourChanged(QColor)), GUI, SLOT(setColour(QColor)) );

		connect( GUI, SIGNAL(colourChanged(QColor)), this, SLOT(onColourChanged(QColor)) );
	}

	return( GUI );
}
