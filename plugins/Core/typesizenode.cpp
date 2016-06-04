#include "typesizenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

TypeSizeNode::TypeSizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_TYPE,	"5e31076f-e01d-45f3-8e07-589e09fe0384" );
	FUGID( PIN_SIZE,	"92ab0042-d6f2-4721-babe-fe9965bacdd4" );

	mValInputType = pinInput<fugio::ChoiceInterface *>( "Type", mPinInputType, PID_CHOICE, PIN_TYPE );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( "Size", mPinOutputSize, PID_INTEGER, PIN_SIZE );

	static QMap<QString,QMetaType::Type>		TypeMap;

	if( TypeMap.isEmpty() )
	{
		for( int t = QMetaType::UnknownType ; t < QMetaType::User ; t++ )
		{
			QString			TypeName = QMetaType::typeName( t );

			if( !TypeName.isEmpty() )
			{
				TypeMap.insert( TypeName, QMetaType::Type( t ) );
			}
		}
	}

	mValInputType->setChoices( TypeMap.keys() );
}

void TypeSizeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString			TypeName = variant( mPinInputType ).toString();
	int				TypeId   = QMetaType::type( TypeName.toLatin1().constData() );
	int				TypeSize = QMetaType::sizeOf( TypeId );

	if( mValOutputSize->variant().toInt() != TypeSize )
	{
		mValOutputSize->setVariant( TypeSize );

		pinUpdated( mPinOutputSize );
	}
}
