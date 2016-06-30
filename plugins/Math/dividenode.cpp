#include "dividenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

DivideNode::DivideNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinNumerator   = pinInput( "Input" );
	mPinDenominator = pinInput( "Input" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_FLOAT );
}

void DivideNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	double				 Num, Den, Res;

	Num = variant( mPinNumerator ).toDouble();
	Den = variant( mPinDenominator ).toDouble();

	if( Den == 0 )
	{
		Res = std::numeric_limits<double>::quiet_NaN();

		return;
	}
	else
	{
		Res = Num / Den;
	}

	mValOutput->setVariant( Res );

	pinUpdated( mPinOutput );
}
