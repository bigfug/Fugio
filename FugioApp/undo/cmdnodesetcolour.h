#ifndef CMDNODESETCOLOUR_H
#define CMDNODESETCOLOUR_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "nodeitem.h"

class CmdNodeSetColour : public QUndoCommand
{
public:
	explicit CmdNodeSetColour( QSharedPointer<NodeItem> pNodeItem, const QColor &pNewVal )
		: mNodeItem( pNodeItem ), mValOld( pNodeItem->colour() ), mValNew( pNewVal )
	{
		setText( QObject::tr( "Set Node Colour" ) );
	}

	virtual ~CmdNodeSetColour( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<NodeItem>	NI = mNodeItem.toStrongRef();

		if( NI )
		{
			NI->setColour( mValOld );
		}
	}

	virtual void redo( void )
	{
		QSharedPointer<NodeItem>	NI = mNodeItem.toStrongRef();

		if( NI )
		{
			NI->setColour( mValNew );
		}
	}

private:
	QWeakPointer<NodeItem>		 mNodeItem;
	QColor						 mValOld, mValNew;
};

#endif // CMDNODESETCOLOUR_H
