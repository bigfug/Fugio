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
	explicit CmdSetColour( const QColor &pColour, QList<NodeItem *> pNodeList, QList<NodeItem *> pGroupList, QList<LinkItem *> pLinkList, QList<NoteItem *> pNoteList )
		: mColour( pColour )
	{
		setText( QObject::tr( "Change colour of Nodes/Links" ) );

		for( NodeItem *I : pNodeList )
		{
			mNodeMap.insert( I, I->colour() );
		}

		for( NodeItem *I : pGroupList )
		{
			mGroupMap.insert( I, I->colour() );
		}

		for( LinkItem *I : pLinkList )
		{
			mLinkMap.insert( I, I->colour() );
		}

		for( NoteItem *N : pNoteList )
		{
			mNoteMap.insert( N, N->backgroundColour() );
		}
	}

	virtual ~CmdSetColour( void )
	{

	}

	virtual void undo( void )
	{
		for( QMap<NodeItem *,QColor>::iterator it = mNodeMap.begin() ; it != mNodeMap.end() ; it++ )
		{
			it.key()->setColour( it.value() );
		}

		for( QMap<NodeItem *,QColor>::iterator it = mGroupMap.begin() ; it != mGroupMap.end() ; it++ )
		{
			it.key()->setColour( it.value() );
		}

		for( QMap<LinkItem *,QColor>::iterator it = mLinkMap.begin() ; it != mLinkMap.end() ; it++ )
		{
			it.key()->setColour( it.value() );
		}

		for( QMap<NoteItem *,QColor>::iterator it = mNoteMap.begin() ; it != mNoteMap.end() ; it++ )
		{
			it.key()->setBackgroundColour( it.value() );

			it.key()->update();
		}
	}

	virtual void redo( void )
	{
		for( QMap<NodeItem *,QColor>::iterator it = mNodeMap.begin() ; it != mNodeMap.end() ; it++ )
		{
			it.key()->setColour( mColour );
		}

		for( QMap<NodeItem *,QColor>::iterator it = mGroupMap.begin() ; it != mGroupMap.end() ; it++ )
		{
			it.key()->setColour( mColour );
		}

		for( QMap<LinkItem *,QColor>::iterator it = mLinkMap.begin() ; it != mLinkMap.end() ; it++ )
		{
			it.key()->setColour( mColour );
		}

		for( QMap<NoteItem *,QColor>::iterator it = mNoteMap.begin() ; it != mNoteMap.end() ; it++ )
		{
			it.key()->setBackgroundColour( mColour );

			it.key()->update();
		}
	}

private:
	QColor									 mColour;

	QMap<NodeItem *,QColor>					 mNodeMap;
	QMap<NodeItem *,QColor>					 mGroupMap;
	QMap<LinkItem *,QColor>					 mLinkMap;
	QMap<NoteItem *,QColor>					 mNoteMap;
};

#endif // CMDSETCOLOUR_H

