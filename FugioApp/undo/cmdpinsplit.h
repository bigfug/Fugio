#ifndef CMDPINSPLIT_H
#define CMDPINSPLIT_H

#include <QUndoCommand>

#include "contextprivate.h"

class CmdPinSplit : public QUndoCommand
{
public:
	explicit CmdPinSplit( QSharedPointer<fugio::ContextInterface> pContext, QSharedPointer<fugio::PinInterface> pPin, const QUuid &pControlId )
		: mContext( pContext ), mPin( pPin ), mControlUuid( pControlId )
	{
		setText( QObject::tr( "Pin Split" ) );
	}

	virtual ~CmdPinSplit( void )
	{

	}

	virtual void undo( void )
	{
		if( !mNodePinId.isNull() )
		{
			mContext->disconnectPins( mPin->globalId(), mNodePinId );
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
			mNode = mContext->global()->createNode( QObject::tr( "Split" ), QUuid::createUuid(), mControlUuid );

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
			mContext->registerNode( mNode, mNode->uuid() );

			if( mNodePinId.isNull() )
			{
				for( QSharedPointer<fugio::PinInterface> Pin : mNode->enumInputPins() )
				{
					if( Pin->inputTypes().contains( mPin->controlUuid() ) )
					{
						mNodePinId = Pin->globalId();

						break;
					}
				}
			}

			if( !mNodePinId.isNull() )
			{
				mContext->connectPins( mPin->globalId(), mNodePinId );
			}
		}
	}

private:
	QSharedPointer<fugio::ContextInterface>		 mContext;
	QSharedPointer<fugio::NodeInterface>		 mNode;
	QSharedPointer<fugio::PinInterface>			 mPin;
	QUuid										 mControlUuid;
	QUuid										 mNodePinId;
};

#endif // CMDPINSPLIT_H
