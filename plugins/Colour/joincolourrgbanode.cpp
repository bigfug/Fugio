#include "joincolourrgbanode.h"

#include <QSettings>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"

JoinColourRGBANode::JoinColourRGBANode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputRed   = pinInput( "Red" );
	mPinInputGreen = pinInput( "Green" );
	mPinInputBlue  = pinInput( "Blue" );
	mPinInputAlpha = pinInput( "Alpha" );

	mValOutputColour = pinOutput<fugio::ColourInterface *>( "Colour", mPinOutputColour, PID_COLOUR );

	mPinInputRed->setValue( "0.0" );
	mPinInputGreen->setValue( "0.5" );
	mPinInputBlue->setValue( "1.0" );
	mPinInputAlpha->setValue( "1.0" );

	mPinInputRed->setDescription( tr( "The amount of red in the colour (0.0-1.0)" ) );

	mPinInputGreen->setDescription( tr( "The amount of green in the colour (0.0-1.0)" ) );

	mPinInputBlue->setDescription( tr( "The amount of blue in the colour (0.0-1.0)" ) );

	mPinInputAlpha->setDescription( tr( "The alpha (transparancy) of the colour (0.0-1.0)" ) );

	mPinOutputColour->setDescription( tr( "The calculated colour" ) );
}

void JoinColourRGBANode::loadSettings( QSettings &pSettings )
{
	mColour = pSettings.value( "colour", mColour ).value<QColor>();
}

void JoinColourRGBANode::saveSettings( QSettings &pSettings ) const
{
	QVariant		V = mColour;

	pSettings.setValue( "colour", V );
}

bool JoinColourRGBANode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	inputsUpdated( 0 );

	return( true );
}

qreal JoinColourRGBANode::value( QSharedPointer<fugio::PinInterface> pPin, qreal pVal )
{
	bool				 ValOk;
	qreal				 NewVal;

	if( !pPin->isConnected() )
	{
		NewVal = pPin->value().toDouble( &ValOk );
	}
	else
	{
		fugio::VariantInterface	*IntVar = qobject_cast<fugio::VariantInterface *>( pPin->connectedPin()->control()->qobject() );

		if( IntVar == 0 )
		{
			return( pVal );
		}

		NewVal = IntVar->variant().toReal( &ValOk );
	}

	if( !ValOk )
	{
		return( pVal );
	}

	NewVal = qBound( 0.0, NewVal, 1.0 );

	return( NewVal );
}

void JoinColourRGBANode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal		R = value( mPinInputRed, mR );
	qreal		G = value( mPinInputGreen, mG );
	qreal		B = value( mPinInputBlue, mB );
	qreal		A = value( mPinInputAlpha, mA );

	if( mR == R && mG == G && mB == B && mA == A )
	{
		return;
	}

	QColor		C = QColor::fromRgbF( R, G, B, A );

	if( !C.isValid() || C == mColour )
	{
		return;
	}

	mColour = C;

	mValOutputColour->setColour( C );

	mNode->context()->pinUpdated( mPinOutputColour );
}
