#ifndef CMDPINJOIN_H
#define CMDPINJOIN_H

#include <QUndoCommand>

#include "contextprivate.h"

class CmdPinJoin : public QUndoCommand
{
public:
	explicit CmdPinJoin( QSharedPointer<fugio::ContextInterface> pContext, QSharedPointer<fugio::PinInterface> pPin, const QUuid &pControlId )
		: mContext( pContext ), mPin( pPin ), mControlId( pControlId )
	{
		setText( QObject::tr( "Pin Join" ) );
	}

	virtual ~CmdPinJoin( void )
	{

	}

	virtual void undo( void )
	{
		if( !mNodePinId.isNull() )
		{
			mContext->disconnectPins( mNodePinId, mPin->globalId() );
		}

		if( mNode )
		{
			mContext->unregisterNode( mNode->uuid() );
		}
	}

	virtual void redo( void )
	{
		if( !mNode )
		{
			mNode = mContext->global()->createNode( QObject::tr( "Join" ), QUuid::createUuid(), mControlId );

			if( mNode )
			{
				if( mNode->control().isNull() )
				{
					mNode.clear();
				}
			}
		}

		if( mNode )
		{
			mContext->registerNode( mNode );

			if( mNodePinId.isNull() )
			{
				for( QSharedPointer<fugio::PinInterface> Pin : mNode->enumOutputPins() )
				{
					if( mPin->inputTypes().contains( Pin->controlUuid() ) )
					{
						mNodePinId = Pin->globalId();

						break;
					}
				}
			}

			if( !mNodePinId.isNull() )
			{
				mContext->connectPins( mNodePinId, mPin->globalId() );
			}
		}
	}

private:
	QSharedPointer<fugio::ContextInterface>		 mContext;
	QSharedPointer<fugio::NodeInterface>		 mNode;
	QSharedPointer<fugio::PinInterface>			 mPin;
	QUuid										 mControlId;
	QUuid										 mNodePinId;
};

#endif // CMDPINJOIN_H
