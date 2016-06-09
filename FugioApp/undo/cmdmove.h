#ifndef CMDMOVE_H
#define CMDMOVE_H

#include <QUndoCommand>
#include <QWeakPointer>

#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "nodeitem.h"
#include "noteitem.h"
#include "contextview.h"

class CmdMove : public QUndoCommand
{
public:
	explicit CmdMove( ContextView *pContext, NodeMoveData &pNodeData, NoteMoveData &pNoteData )
		: mContext( pContext ), mNodeData( pNodeData )
	{
		setText( QObject::tr( "Move" ) );

		for( NoteMoveData::iterator it = pNoteData.begin() ; it != pNoteData.end() ; it++ )
		{
			mNoteData << NoteData( it.key(), it.value() );
		}
	}

	virtual ~CmdMove( void )
	{

	}

	virtual void undo( void )
	{
		for( NodeMoveData::iterator it = mNodeData.begin() ; it != mNodeData.end() ; it++ )
		{
			QSharedPointer<NodeItem>	NI = mContext->findNodeItem( it.key() );

			if( NI && NI->pos() != it.value().first )
			{
				setNodePos( NI, it.value().first );
			}
		}

		for( NoteData &ND : mNoteData )
		{
			QSharedPointer<NoteItem>	NI = ND.first.toStrongRef();

			if( NI && NI->pos() != ND.second.first )
			{
				NI->setPos( ND.second.first );
			}
		}
	}

	virtual void redo( void )
	{
		for( NodeMoveData::iterator it = mNodeData.begin() ; it != mNodeData.end() ; it++ )
		{
			QSharedPointer<NodeItem>	NI = mContext->findNodeItem( it.key() );

			if( NI && NI->pos() != it.value().second )
			{
				setNodePos( NI, it.value().second );
			}
		}

		for( NoteData &ND : mNoteData )
		{
			QSharedPointer<NoteItem>	NI = ND.first.toStrongRef();

			if( NI && NI->pos() != ND.second.second )
			{
				NI->setPos( ND.second.second );
			}
		}
	}

private:
	void setNodePos( QSharedPointer<NodeItem> NI, const QPointF &pPos ) const
	{
		if( NI )
		{
			NI->setFlags( NI->flags() & ~QGraphicsItem::ItemSendsGeometryChanges );

			NI->setNodePos( pPos );

			NI->setFlags( NI->flags() | QGraphicsItem::ItemSendsGeometryChanges );
		}
	}

private:
	ContextView								*mContext;
	NodeMoveData							 mNodeData;

	typedef QPair<QWeakPointer<NoteItem>,MoveData>	NoteData;

	QList<NoteData>							 mNoteData;
};

#endif // CMDMOVE_H

