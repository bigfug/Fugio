#include "numberrangenode.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>

#include <QDoubleSpinBox>
#include <QDateTime>
#include <QSettings>
#include <QFormLayout>

#include <limits>

NumberRangeNode::NumberRangeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputValue = pinInput( "Input Value" );

	mPinInputMinInput  = pinInput( "Input Min" );
	mPinInputMaxInput  = pinInput( "Input Max" );
	mPinInputMinOutput = pinInput( "Output Min" );
	mPinInputMaxOutput = pinInput( "Output Max" );

	mPinInputMinInput->setValue( 0 );
	mPinInputMaxInput->setValue( 1 );
	mPinInputMinOutput->setValue( 0 );
	mPinInputMaxOutput->setValue( 1 );

	mValue = pinOutput<fugio::VariantInterface *>( "Output Value", mPinOutputValue, PID_FLOAT );
}

NumberRangeNode::~NumberRangeNode( void )
{
}

void NumberRangeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	double		ValI = variant( mPinInputValue ).toDouble();
	double		MinI = variant( mPinInputMinInput ).toDouble();
	double		MaxI = variant( mPinInputMaxInput ).toDouble();
	double		MinO = variant( mPinInputMinOutput ).toDouble();
	double		MaxO = variant( mPinInputMaxOutput ).toDouble();

	double		ValO = 0;

	ValI = qBound( MinI, ValI, MaxI );

	double		RngI = MaxI - MinI;
	double		RngO = MaxO - MinO;

	ValI = ( ValI - MinI ) / RngI;

	ValO = ( ValI * RngO ) + MinO;

	if( ValO != mValue->variant().toDouble() )
	{
		mValue->setVariant( ValO );

		pinUpdated( mPinOutputValue );
	}
}
