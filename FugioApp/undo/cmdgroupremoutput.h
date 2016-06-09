#ifndef CMDGROUPREMOUTPUT_H
#define CMDGROUPREMOUTPUT_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "../contextview.h"
#include "../nodeitem.h"

class CmdGroupRemOutput : public QUndoCommand
{
public:
	explicit CmdGroupRemOutput( ContextView *pContextView, const QUuid &pGroupId, QSharedPointer<fugio::PinInterface> pPin )
		: mContextView( pContextView ), mGroupId( pGroupId ), mPin( pPin )
	{
		setText( QObject::tr( "Group Remove Output" ) );
	}

	virtual ~CmdGroupRemOutput( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mGroupId );

		if( NI )
		{
			NI->pinOutputAdd( mPin );

			NI->layoutPins();
		}
	}

	virtual void redo( void )
	{
		QSharedPointer<NodeItem>	NI = mContextView->findNodeItem( mGroupId );

		if( NI )
		{
			NI->pinOutputRemove( mPin->globalId() );

			NI->layoutPins();
		}
	}

private:
	ContextView								*mContextView;
	QUuid									 mGroupId;
	QSharedPointer<fugio::PinInterface>		 mPin;
};

#endif // CMDGROUPREMOUTPUT_H
