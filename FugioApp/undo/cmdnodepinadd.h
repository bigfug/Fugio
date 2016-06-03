#ifndef CMDNODEPINADD_H
#define CMDNODEPINADD_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

class CmdNodePinAdd : public QUndoCommand
{
public:
	explicit CmdNodePinAdd( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
		: mNode( pNode ), mPin( pPin )
	{
		setText( QObject::tr( "Add pin" ) );
	}

	virtual ~CmdNodePinAdd( void )
	{

	}

	virtual void undo( void )
	{
		mNode->removePin( mPin );
	}

	virtual void redo( void )
	{
		mNode->addPin( mPin );
	}

private:
	QSharedPointer<fugio::NodeInterface>	mNode;
	QSharedPointer<fugio::PinInterface>	mPin;
};

#endif // CMDNODEPINADD_H
