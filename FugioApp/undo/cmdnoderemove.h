#ifndef CMDNODEREMOVE_H
#define CMDNODEREMOVE_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

class CmdNodeRemove : public QUndoCommand
{
public:
	explicit CmdNodeRemove( QSharedPointer<fugio::ContextInterface> pContext, QSharedPointer<fugio::NodeInterface> pNode )
		: mContext( pContext ), mNode( pNode )
	{
		setText( QObject::tr( "Remove Node" ) );
	}

	virtual ~CmdNodeRemove( void )
	{

	}

	virtual void undo( void )
	{
		mContext->registerNode( mNode, mNode->uuid() );

		foreach( auto PinCon, mPinLst )
		{
			mContext->connectPins( PinCon.first, PinCon.second );
		}
	}

	virtual void redo( void )
	{
		foreach( auto PinPtr, mNode->enumInputPins() )
		{
			foreach( auto SrcPin, PinPtr->connectedPins() )
			{
				mPinLst.append( QPair<QUuid,QUuid>( SrcPin->globalId(), PinPtr->globalId() ) );
			}

			mContext->disconnectPin( PinPtr->globalId() );
		}

		foreach( auto PinPtr, mNode->enumOutputPins() )
		{
			foreach( auto DstPin, PinPtr->connectedPins() )
			{
				mPinLst.append( QPair<QUuid,QUuid>( PinPtr->globalId(), DstPin->globalId() ) );
			}

			mContext->disconnectPin( PinPtr->globalId() );
		}

		mContext->unregisterNode( mNode->uuid() );
	}

private:
	QSharedPointer<fugio::ContextInterface>		 mContext;
	QSharedPointer<fugio::NodeInterface>			 mNode;
	QString									 mName;
	QUuid									 mUuid;
	QList< QPair<QUuid,QUuid> >				 mPinLst;
};

#endif // CMDNODEREMOVE_H
