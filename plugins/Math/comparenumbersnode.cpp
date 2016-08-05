#include "comparenumbersnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

CompareNumbersNode::CompareNumbersNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{608ac771-490b-4ae6-9c81-12b9af526d09}" );

	mPinInput1 = pinInput( "Number 1", PII_NUMBER1 );
	mPinInput2 = pinInput( "Number 2", PII_NUMBER2 );

	mValEqual  = pinOutput<fugio::VariantInterface *>( "Equal", mPinEqual, PID_BOOL );
	mValFuzzy  = pinOutput<fugio::VariantInterface *>( "Fuzzy", mPinFuzzy, PID_BOOL );
	mValHigher = pinOutput<fugio::VariantInterface *>( "N1>N2", mPinHigher, PID_BOOL );
}

void CompareNumbersNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	qreal		v1 = variant( mPinInput1 ).toReal();
	qreal		v2 = variant( mPinInput2 ).toReal();

	bool		Eq = bool( v1 == v2 );
	bool		Fz = qFuzzyCompare( v1, v2 );
	bool		Gt = bool( v1 > v2 );

	if( !pTimeStamp || Eq != mValEqual->variant().toBool() )
	{
		mValEqual->setVariant( Eq );

		pinUpdated( mPinEqual );
	}

	if( !pTimeStamp || Fz != mValFuzzy->variant().toBool() )
	{
		mValFuzzy->setVariant( Fz );

		pinUpdated( mPinFuzzy );
	}

	if( !pTimeStamp || Gt != mValHigher->variant().toBool() )
	{
		mValHigher->setVariant( Gt );

		pinUpdated( mPinHigher );
	}
}
