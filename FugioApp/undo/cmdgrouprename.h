#ifndef CMDGROUPRENAME_H
#define CMDGROUPRENAME_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "../contextview.h"
#include "../nodeitem.h"

class CmdGroupRename : public QUndoCommand
{
public:
	explicit CmdGroupRename( ContextView *pContextView, const QUuid &pNodeId, const QString &pOldVal, const QString &pNewVal )
		: mContextView( pContextView ), mNodeId( pNodeId ), mValOld( pOldVal ), mValNew( pNewVal )
	{
		setText( QObject::tr( "Rename Group" ) );
	}

	virtual ~CmdGroupRename( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mNodeId );

		if( NI )
		{
			NI->setName( mValOld );
		}
	}

	virtual void redo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mNodeId );

		if( NI )
		{
			NI->setName( mValNew );
		}
	}

private:
	ContextView			*mContextView;
	QUuid				 mNodeId;
	QString				 mValOld, mValNew;
};

#endif // CMDGROUPRENAME_H
