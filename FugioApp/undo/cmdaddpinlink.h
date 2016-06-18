#ifndef CMDADDPINLINK_H
#define CMDADDPINLINK_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include <fugio/node_control_interface.h>

#include "app.h"

class CmdAddPinLink : public QUndoCommand
{
public:
	explicit CmdAddPinLink( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pSrcPin )
		: mNode( pNode ), mSrcPin( pSrcPin )
	{
		setText( QObject::tr( "Add link to new pin" ) );
	}

	virtual ~CmdAddPinLink( void )
	{

	}

	virtual void undo( void )
	{
		mNode->context()->disconnectPins( mSrcPin->globalId(), mDstPin->globalId() );

		mNode->removePin( mDstPin );
	}

	virtual void redo( void )
	{
		if( !mDstPin )
		{
			QUuid		PinControlUuid;

			if( mNode->hasControl() )
			{
				PinControlUuid = mNode->control()->pinAddControlUuid( mSrcPin.data() );
			}

			mNode->createPin( mSrcPin->name(), mSrcPin->direction() == PIN_INPUT ? PIN_OUTPUT : PIN_INPUT, QUuid::createUuid(), mDstPin, PinControlUuid );

			if( !mDstPin )
			{
				return;
			}

			if( mDstPin->direction() == PIN_INPUT && mNode->control() && mNode->control()->pinShouldAutoRename( mDstPin.data() ) )
			{
				mDstPin->setAutoRename( true );
			}

			mDstPin->setRemovable( true );
		}
		else
		{
			mNode->addPin( mDstPin );
		}

		mNode->context()->connectPins( mSrcPin->globalId(), mDstPin->globalId() );

		if( mDstPin->updatable() )
		{
			mNode->context()->pinUpdated( mDstPin );
		}
	}

private:
	QSharedPointer<fugio::NodeInterface>	 mNode;
	QSharedPointer<fugio::PinInterface>		 mSrcPin;
	QSharedPointer<fugio::PinInterface>		 mDstPin;
};

#endif // CMDADDPINLINK_H

