#ifndef CMDGROUPPOP_H
#define CMDGROUPPOP_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "../contextview.h"
#include "../nodeitem.h"

class CmdGroupPop : public QUndoCommand
{
public:
	explicit CmdGroupPop( ContextView *pContextView )
		: mContextView( pContextView ), mGroupId( pContextView->groupId() )
	{
		setText( QObject::tr( "Group Parent" ) );
	}

	virtual ~CmdGroupPop( void )
	{

	}

	virtual void undo( void )
	{
		mContextView->pushGroup( mGroupId );
	}

	virtual void redo( void )
	{
		mContextView->popGroup();
	}

private:
	ContextView			*mContextView;
	QUuid				 mGroupId;
};

#endif // CMDGROUPPOP_H
