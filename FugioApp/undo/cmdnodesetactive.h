#ifndef CMDNODESETACTIVE_H
#define CMDNODESETACTIVE_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

class CmdNodeSetActive : public QUndoCommand
{
public:
	explicit CmdNodeSetActive( QSharedPointer<fugio::NodeInterface> pNode, bool pNewVal )
		: mNode( pNode ), mValOld( pNode->isActive() ), mValNew( pNewVal )
	{
		setText( mValNew ? QObject::tr( "Activate Node" ) : QObject::tr( "Deactivate Node" ) );
	}

	virtual ~CmdNodeSetActive( void )
	{

	}

	virtual void undo( void )
	{
		mNode->setActive( mValOld );
	}

	virtual void redo( void )
	{
		mNode->setActive( mValNew );
	}

private:
	QSharedPointer<fugio::NodeInterface>			 mNode;
	bool									 mValOld, mValNew;
};

#endif // CMDNODESETACTIVE_H
