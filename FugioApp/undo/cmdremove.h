#ifndef CMDREMOVE_H
#define CMDREMOVE_H

#include <QUndoCommand>
#include <QUuid>
#include <QMultiHash>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "noteitem.h"
#include "contextview.h"

class CmdRemove : public QUndoCommand
{
public:
	explicit CmdRemove( ContextView *pContextView, QList< QSharedPointer<fugio::NodeInterface> > &pNodeList, QList<QSharedPointer<NodeItem>> &pGroupList, QMultiMap<QUuid,QUuid> &pLinkList, QList<QSharedPointer<NoteItem>> &pNoteList, QMap<QUuid,QUuid> pNodeGroups )
		: mContextView( pContextView ), mNodeList( pNodeList ), mGroupList( pGroupList ), mLinkList( pLinkList ), mNoteList( pNoteList ), mNodeGroups( pNodeGroups )
	{
		setText( QObject::tr( "Remove Nodes/Links" ) );

		for( QSharedPointer<fugio::NodeInterface> mNode : mNodeList )
		{
			for( auto PinPtr : mNode->enumInputPins() )
			{
				for( auto SrcPin : PinPtr->connectedPins() )
				{
					if( !mLinkList.contains( SrcPin->globalId(), PinPtr->globalId() ) )
					{
						mLinkList.insert( SrcPin->globalId(), PinPtr->globalId() );
					}
				}
			}

			for( auto PinPtr : mNode->enumOutputPins() )
			{
				for( auto DstPin : PinPtr->connectedPins() )
				{
					if( !mLinkList.contains( PinPtr->globalId(), DstPin->globalId() ) )
					{
						mLinkList.insert( PinPtr->globalId(), DstPin->globalId() );
					}
				}
			}
		}
	}

	virtual ~CmdRemove( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<fugio::ContextInterface>		Context = mContextView->context();

		for( QSharedPointer<NodeItem> G : mGroupList )
		{
			mContextView->nodeAdd( G );
		}

		for( QSharedPointer<fugio::NodeInterface> mNode : mNodeList )
		{
			Context->registerNode( mNode, mNode->uuid() );
		}

		for( QMap<QUuid,QUuid>::const_iterator it = mNodeGroups.begin() ; it != mNodeGroups.end() ; it++ )
		{
			QSharedPointer<NodeItem> N = mContextView->findNodeItem( it.key() );

			if( N )
			{
				N->setGroupId( it.value() );
			}
		}

		for( QUuid Src : mLinkList.keys() )
		{
			for( QUuid Dst : mLinkList.values( Src ) )
			{
				Context->connectPins( Src, Dst );
			}
		}

		for( QSharedPointer<NoteItem> P : mNoteList )
		{
			if( P )
			{
				mContextView->noteAdd( P );
			}
		}

		mContextView->updateItemVisibility();
	}

	virtual void redo( void )
	{
		QSharedPointer<fugio::ContextInterface>		Context = mContextView->context();

		for( QUuid Src : mLinkList.keys() )
		{
			for( QUuid Dst : mLinkList.values( Src ) )
			{
				Context->disconnectPins( Src, Dst );
			}
		}

		for( QSharedPointer<fugio::NodeInterface> mNode : mNodeList )
		{
			Context->unregisterNode( mNode->uuid() );
		}

		for( QSharedPointer<NoteItem> P : mNoteList )
		{
			if( P )
			{
				mContextView->noteRemove( P );
			}
		}

		for( QSharedPointer<NodeItem> G : mGroupList )
		{
			mContextView->nodeRemove( G );
		}
	}

private:
	ContextView										*mContextView;
	QList< QSharedPointer<fugio::NodeInterface> >	 mNodeList;
	QList<QSharedPointer<NodeItem>>					 mGroupList;
	QMultiMap<QUuid,QUuid>							 mLinkList;
	QList<QSharedPointer<NoteItem>>					 mNoteList;
	QMap<QUuid,QUuid>								 mNodeGroups;
};

#endif // CMDREMOVE_H

