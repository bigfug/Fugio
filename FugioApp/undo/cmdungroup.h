#ifndef CMDUNGROUP_H
#define CMDUNGROUP_H

#include <QUndoCommand>
#include <QUuid>
#include <QMultiHash>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "noteitem.h"
#include "contextview.h"

class CmdUngroup : public QUndoCommand
{
public:
	explicit CmdUngroup( ContextView *pContextView, const QString &pGroupName, QList<NodeItem *> &pNodeList, QList<NoteItem *> &pNoteList )
		: mContextView( pContextView ), mGroupName( pGroupName ), mNodeList( pNodeList ), mNoteList( pNoteList )
	{
		setText( QObject::tr( "Ungroup" ) );
	}

	virtual ~CmdUngroup( void )
	{

	}

	virtual void undo( void )
	{
		mGroupId = mContextView->group( mGroupName, mNodeList, mNoteList, mGroupId );
	}

	virtual void redo( void )
	{
		mContextView->ungroup( mNodeList, mNoteList, mGroupId );
	}

private:
	ContextView								*mContextView;
	QString									 mGroupName;
	QList<NodeItem *>						 mNodeList;
	QList<NoteItem *>						 mNoteList;
	QUuid									 mGroupId;
};

#endif // CMDUNGROUP_H
