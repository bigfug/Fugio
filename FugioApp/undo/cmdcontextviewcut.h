#ifndef CMDCONTEXTVIEWCUT_H
#define CMDCONTEXTVIEWCUT_H

#include <QUndoCommand>
#include <QUuid>
#include <QMultiHash>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "noteitem.h"
#include "contextview.h"

class CmdContextViewCut : public QUndoCommand
{
public:
	explicit CmdContextViewCut( QSharedPointer<fugio::ContextInterface> pContext, QList< QSharedPointer<fugio::NodeInterface> > &pNodeList, QMultiMap<QUuid,QUuid> &pLinkList, QList<QWeakPointer<NoteItem>> &pNoteList )
		: mContext( pContext ), mNodeList( pNodeList ), mLinkList( pLinkList ), mNoteList( pNoteList )
	{
		setText( QObject::tr( "Cut" ) );

		foreach( QSharedPointer<fugio::NodeInterface> mNode, mNodeList )
		{
			foreach( auto PinPtr, mNode->enumInputPins() )
			{
				foreach( auto SrcPin, PinPtr->connectedPins() )
				{
					if( !mLinkList.contains( SrcPin->globalId(), PinPtr->globalId() ) )
					{
						mLinkList.insert( SrcPin->globalId(), PinPtr->globalId() );
					}
				}
			}

			foreach( auto PinPtr, mNode->enumOutputPins() )
			{
				foreach( auto DstPin, PinPtr->connectedPins() )
				{
					if( !mLinkList.contains( PinPtr->globalId(), DstPin->globalId() ) )
					{
						mLinkList.insert( PinPtr->globalId(), DstPin->globalId() );
					}
				}
			}
		}
	}

	virtual ~CmdContextViewCut( void )
	{

	}

	virtual void undo( void )
	{
		for( QSharedPointer<fugio::NodeInterface> mNode : mNodeList )
		{
			mContext->registerNode( mNode, mNode->uuid() );
		}

		foreach( QUuid Src, mLinkList.keys() )
		{
			foreach( QUuid Dst, mLinkList.values( Src ) )
			{
				mContext->connectPins( Src, Dst );
			}
		}

		for( QWeakPointer<NoteItem> &N : mNoteList )
		{
			QSharedPointer<NoteItem>	P = N.toStrongRef();

			if( P )
			{
				P->view()->scene()->addItem( P.data() );
			}
		}
	}

	virtual void redo( void )
	{
		foreach( QUuid Src, mLinkList.keys() )
		{
			foreach( QUuid Dst, mLinkList.values( Src ) )
			{
				mContext->disconnectPins( Src, Dst );
			}
		}

		foreach( QSharedPointer<fugio::NodeInterface> mNode, mNodeList )
		{
			mContext->unregisterNode( mNode->uuid() );
		}

		for( QWeakPointer<NoteItem> &N : mNoteList )
		{
			QSharedPointer<NoteItem>	P = N.toStrongRef();

			if( P )
			{
				P->view()->scene()->removeItem( P.data() );
			}
		}
	}

private:
	QSharedPointer<fugio::ContextInterface>		 mContext;
	QList< QSharedPointer<fugio::NodeInterface> >	 mNodeList;
	QMultiMap<QUuid,QUuid>					 mLinkList;
	QList<QWeakPointer<NoteItem>>			 mNoteList;
};

#endif // CMDCONTEXTVIEWCUT_H

