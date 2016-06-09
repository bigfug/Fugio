#ifndef CMDGROUP_H
#define CMDGROUP_H

#include <QUndoCommand>
#include <QUuid>
#include <QMultiHash>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "noteitem.h"
#include "contextview.h"
#include "app.h"

class CmdGroup : public QUndoCommand
{
public:
	explicit CmdGroup( ContextView *pContextView, const QString &pGroupName, QList<NodeItem *> &pNodeList, QList<NoteItem *> &pNoteList )
		: mContextView( pContextView ), mGroupName( pGroupName ), mNodeList( pNodeList ), mNoteList( pNoteList )
	{
		setText( QObject::tr( "Group" ) );

		gApp->incrementStatistic( "group-added" );
	}

	virtual ~CmdGroup( void )
	{

	}

	virtual void undo( void )
	{
		mContextView->ungroup( mNodeList, mNoteList, mGroupId );
	}

	virtual void redo( void )
	{
		mGroupId = mContextView->group( mGroupName, mNodeList, mNoteList, mGroupId );
	}

private:
	ContextView								*mContextView;
	QString									 mGroupName;
	QList<NodeItem *>						 mNodeList;
	QList<NoteItem *>						 mNoteList;
	QUuid									 mGroupId;
};

#endif // CMDGROUP_H
