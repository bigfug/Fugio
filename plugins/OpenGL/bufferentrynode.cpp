#include "bufferentrynode.h"

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/context_interface.h>

BufferEntryNode::TypeMap		BufferEntryNode::mTypeMap;

BufferEntryNode::BufferEntryNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_TYPE,	"5e31076f-e01d-45f3-8e07-589e09fe0384" );
	FUGID( PIN_COUNT,	"a19fd35b-511d-4584-bae9-675daf4d11f0" );
	FUGID( PIN_ENTRY,	"dcb4e7ad-d612-4d16-a13c-686012f72e3a" );
	FUGID( PIN_SIZE,	"92ab0042-d6f2-4721-babe-fe9965bacdd4" );

	mValInputType = pinInput<fugio::ChoiceInterface *>( "Type", mPinInputType, PID_CHOICE, PIN_TYPE );

	mPinInputCount = pinInput( "Count", PIN_COUNT );

	mPinInputCount->setValue( 1 );

	mValOutputEntry = pinOutput<BufferEntryPin *>( "Entry", mPinOutputEntry, PID_OPENGL_BUFFER_ENTRY, PIN_ENTRY );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( "Size", mPinOutputSize, PID_INTEGER, PIN_SIZE );

	if( mTypeMap.isEmpty() )
	{
		mTypeMap.insert( "GL_BYTE", TypeData( GL_BYTE, 1 ) );
		mTypeMap.insert( "GL_UNSIGNED_BYTE", TypeData( GL_UNSIGNED_BYTE, 1 ) );
		mTypeMap.insert( "GL_SHORT", TypeData( GL_SHORT, 2 ) );
		mTypeMap.insert( "GL_UNSIGNED_SHORT", TypeData( GL_UNSIGNED_SHORT, 2 ) );
		mTypeMap.insert( "GL_INT", TypeData( GL_INT, 4 ) );
		mTypeMap.insert( "GL_UNSIGNED_INT", TypeData( GL_UNSIGNED_INT, 4 ) );
		mTypeMap.insert( "GL_HALF_FLOAT", TypeData( GL_HALF_FLOAT, 2 ) );
		mTypeMap.insert( "GL_FLOAT", TypeData( GL_FLOAT, 4 ) );
		mTypeMap.insert( "GL_DOUBLE", TypeData( GL_DOUBLE, 8 ) );
		mTypeMap.insert( "GL_FIXED", TypeData( GL_FIXED, 4 ) );
		mTypeMap.insert( "GL_INT_2_10_10_10_REV", TypeData( GL_INT_2_10_10_10_REV, 4 ) );
		mTypeMap.insert( "GL_UNSIGNED_INT_2_10_10_10_REV", TypeData( GL_UNSIGNED_INT_2_10_10_10_REV, 4 ) );
		mTypeMap.insert( "GL_UNSIGNED_INT_10F_11F_11F_REV", TypeData( GL_UNSIGNED_INT_10F_11F_11F_REV, 4 ) );
	}

	mValInputType->setChoices( mTypeMap.keys() );
}

void BufferEntryNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString		Name = variant( mPinInputType ).toString();
	TypeData	Data = mTypeMap.value( Name );

	mValOutputEntry->setType( Data.first );
	mValOutputEntry->setNormalised( GL_FALSE );
	mValOutputEntry->setSize( variant( mPinInputCount ).toInt() );

	pinUpdated( mPinOutputEntry );

	if( mValOutputSize->variant().toUInt() != mValOutputEntry->entrySize() )
	{
		mValOutputSize->setVariant( mValOutputEntry->entrySize() );

		pinUpdated( mPinOutputSize );
	}
}
