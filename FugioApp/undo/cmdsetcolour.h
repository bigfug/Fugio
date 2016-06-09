#ifndef CMDSETCOLOUR_H
#define CMDSETCOLOUR_H

#include <QUndoCommand>
#include <QUuid>
#include <QMultiHash>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "noteitem.h"
#include "contextview.h"

class CmdSetColour : public QUndoCommand
{
public:
	explicit CmdSetColour( QList<QWeakPointer<NodeItem>> &pNodeList, QList<QWeakPointer<LinkItem>> &pLinkList, QList<QWeakPointer<NoteItem>> &pNoteList, const QColor &pColour )
		: mColour( pColour )
	{
		setText( QObject::tr( "Change colour of Nodes/Links" ) );

		for( QWeakPointer<NodeItem> W : pNodeList )
		{
			QSharedPointer<NodeItem>	 P = W.toStrongRef();

			if( !P )
			{
				continue;
			}

			mNodeList << QPair<QWeakPointer<NodeItem>,QColor>( W, P->colour() );
		}

		for( QWeakPointer<LinkItem> W : pLinkList )
		{
			QSharedPointer<LinkItem>	 P = W.toStrongRef();

			if( !P )
			{
				continue;
			}

			mLinkList << QPair<QWeakPointer<LinkItem>,QColor>( W, P->colour() );
		}

		for( QWeakPointer<NoteItem> W : pNoteList )
		{
			QSharedPointer<NoteItem>	 P = W.toStrongRef();

			if( !P )
			{
				continue;
			}

			mNoteList << QPair<QWeakPointer<NoteItem>,QColor>( W, P->backgroundColour() );
		}
	}

	virtual ~CmdSetColour( void )
	{

	}

	virtual void undo( void )
	{
		for( QPair<QWeakPointer<NodeItem>,QColor> E : mNodeList )
		{
			QSharedPointer<NodeItem>	 P = E.first.toStrongRef();

			if( !P )
			{
				continue;
			}

			P->setColour( E.second );
		}

		for( QPair<QWeakPointer<LinkItem>,QColor> E : mLinkList )
		{
			QSharedPointer<LinkItem>	 P = E.first.toStrongRef();

			if( !P )
			{
				continue;
			}

			P->setColour( E.second );
		}

		for( QPair<QWeakPointer<NoteItem>,QColor> E : mNoteList )
		{
			QSharedPointer<NoteItem>	 P = E.first.toStrongRef();

			if( !P )
			{
				continue;
			}

			P->setBackgroundColour( E.second );
		}
	}

	virtual void redo( void )
	{
		for( QPair<QWeakPointer<NodeItem>,QColor> E : mNodeList )
		{
			QSharedPointer<NodeItem>	 P = E.first.toStrongRef();

			if( !P )
			{
				continue;
			}

			P->setColour( mColour );
		}

		for( QPair<QWeakPointer<LinkItem>,QColor> E : mLinkList )
		{
			QSharedPointer<LinkItem>	 P = E.first.toStrongRef();

			if( !P )
			{
				continue;
			}

			P->setColour( mColour );
		}

		for( QPair<QWeakPointer<NoteItem>,QColor> E : mNoteList )
		{
			QSharedPointer<NoteItem>	 P = E.first.toStrongRef();

			if( !P )
			{
				continue;
			}

			P->setBackgroundColour( mColour );
		}
	}

private:
	QList<QPair<QWeakPointer<NodeItem>,QColor>>			 mNodeList;
	QList<QPair<QWeakPointer<LinkItem>,QColor>>			 mLinkList;
	QList<QPair<QWeakPointer<NoteItem>,QColor>>			 mNoteList;
	QColor												 mColour;
};

#endif // CMDSETCOLOUR_H

