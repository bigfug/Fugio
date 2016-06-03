#ifndef CMDGROUPPUSH_H
#define CMDGROUPPUSH_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "../contextview.h"
#include "../nodeitem.h"

class CmdGroupPush : public QUndoCommand
{
public:
	explicit CmdGroupPush( ContextView *pContextView, const QUuid &pGroupId )
		: mContextView( pContextView ), mGroupId( pGroupId )
	{
		setText( QObject::tr( "Group Push" ) );
	}

	virtual ~CmdGroupPush( void )
	{

	}

	virtual void undo( void )
	{
		mContextView->popGroup();
	}

	virtual void redo( void )
	{
		mContextView->pushGroup( mGroupId );
	}

private:
	ContextView			*mContextView;
	QUuid				 mGroupId;
};

#endif // CMDGROUPPUSH_H
