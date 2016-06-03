#include "lcdnumbernode.h"

#include <QLCDNumber>

LcdNumberNode::LcdNumberNode(QSharedPointer<fugio::NodeInterface> pNode)
	: NodeControlBase( pNode ), mValInput( 0 )
{
	mPinInput = pinInput( "Number" );
}

LcdNumberNode::~LcdNumberNode()
{

}

bool LcdNumberNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	inputsUpdated( 0 );

	return( true );
}

QWidget *LcdNumberNode::gui()
{
	QLCDNumber		*GUI = new QLCDNumber();

	GUI->setSegmentStyle( QLCDNumber::Flat );

	GUI->display( mValInput );

	connect( this, SIGNAL(valueUpdated(double)), GUI, SLOT(display(double)) );

	return( GUI );
}

void LcdNumberNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	double		NewVal = variant( mPinInput ).toDouble();

	if( NewVal != mValInput )
	{
		mValInput = NewVal;

		emit valueUpdated( mValInput );
	}
}
