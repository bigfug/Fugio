#include "numberrangelimitnode.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>

#include <QDoubleSpinBox>
#include <QDateTime>
#include <QSettings>
#include <QFormLayout>

#include <limits>

NumberRangeLimitNode::NumberRangeLimitNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputValue = pinInput( "Number" );

	mPinInputMinInput  = pinInput( "Number Min" );
	mPinInputMaxInput  = pinInput( "Number Max" );

	mPinInputMinInput->setValue( 0 );
	mPinInputMaxInput->setValue( 1 );

	mValue = pinOutput<fugio::VariantInterface *>( "Number", mPinOutputValue, PID_FLOAT );
}

NumberRangeLimitNode::~NumberRangeLimitNode( void )
{
}

void NumberRangeLimitNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	double		ValI = variant( mPinInputValue ).toDouble();
	double		MinI = variant( mPinInputMinInput ).toDouble();
	double		MaxI = variant( mPinInputMaxInput ).toDouble();

	ValI = qBound( MinI, ValI, MaxI );

	if( ValI != mValue->variant().toDouble() )
	{
		mValue->setVariant( ValI );

		pinUpdated( mPinOutputValue );
	}
}
