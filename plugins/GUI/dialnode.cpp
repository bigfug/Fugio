#include "dialnode.h"

#include <QSlider>
#include <QSettings>

#include "dialwidget.h"

DialNode::DialNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_VALUE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "", mPinOutputValue, PID_FLOAT, PIN_OUTPUT_VALUE );
}

void DialNode::dialValueChanged( qreal pValue )
{
	if( pValue != mValOutputValue->variant().toFloat() )
	{
		mValOutputValue->setVariant( pValue );

		pinUpdated( mPinOutputValue );
	}
}

void DialNode::loadSettings( QSettings &pSettings)
{
	float		NewVal = pSettings.value( "value", mValOutputValue->variant() ).toFloat();

	if( NewVal != mValOutputValue->variant().toFloat() )
	{
		mValOutputValue->setVariant( NewVal );

		pinUpdated( mPinOutputValue );
	}

	emit valueChanged( int( NewVal * 1000.0f ) );
}

void DialNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "value", mValOutputValue->variant().toFloat() );
}

QGraphicsItem *DialNode::guiItem()
{
	DialWidget		*GUI = new DialWidget();

	if( GUI )
	{
		GUI->setValue( mValOutputValue->variant().toReal() );

		connect( this, SIGNAL(valueChanged(qreal)), GUI, SLOT(setValue(qreal)) );

		connect( GUI, SIGNAL(valueChanged(qreal)), this, SLOT(dialValueChanged(qreal)) );
	}

	return( GUI );
}
