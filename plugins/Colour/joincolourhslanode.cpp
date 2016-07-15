#include "joincolourhslanode.h"

#include <QSettings>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"

JoinColourHSLANode::JoinColourHSLANode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputHue        = pinInput( "Hue" );
	mPinInputSaturation = pinInput( "Saturation" );
	mPinInputLightness  = pinInput( "Lightness" );
	mPinInputAlpha      = pinInput( "Alpha" );

	mValOutputColour = pinOutput<fugio::ColourInterface *>( "Colour", mPinOutputColour, PID_COLOUR );

	mPinInputHue->setDescription( tr( "The hue of the colour (0.0-1.0)" ) );

	mPinInputSaturation->setDescription( tr( "The saturation (gray to colour) of the colour (0.0-1.0)" ) );

	mPinInputLightness->setDescription( tr( "The lightness (black to white) of the colour (0.0-1.0)" ) );

	mPinInputAlpha->setDescription( tr( "The alpha (transparancy) of the colour (0.0-1.0)" ) );

	mPinOutputColour->setDescription( tr( "The calculated colour" ) );

	mPinInputHue->setValue( 0.0f );
	mPinInputSaturation->setValue( 0.5f );
	mPinInputLightness->setValue( 1.0f );
	mPinInputAlpha->setValue( 1.0f );
}

void JoinColourHSLANode::loadSettings( QSettings &pSettings )
{
	mColour = pSettings.value( "colour", mColour ).value<QColor>();
}

void JoinColourHSLANode::saveSettings( QSettings &pSettings ) const
{
	QVariant		V = mColour;

	pSettings.setValue( "colour", V );
}

qreal JoinColourHSLANode::value( QSharedPointer<fugio::PinInterface> pPin, qreal pVal )
{
	bool				 ValOk;
	qreal				 NewVal;

	if( !pPin->isConnected() )
	{
		NewVal = pPin->value().toReal( &ValOk );
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

void JoinColourHSLANode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal		H = value( mPinInputHue, h );
	qreal		S = value( mPinInputSaturation, s );
	qreal		L = value( mPinInputLightness, l );
	qreal		A = value( mPinInputAlpha, a );

	if( h == H && s == S && l == L && a == A )
	{
		return;
	}

	QColor		C = QColor::fromHslF( H, S, L, A );

	if( C == mColour )
	{
		return;
	}

	mColour = C;

	mValOutputColour->setColour( C );

	mNode->context()->pinUpdated( mPinOutputColour );
}
