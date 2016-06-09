#ifndef CMDNODEMOVE_H
#define CMDNODEMOVE_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "nodeitem.h"

class CmdNodeMove : public QUndoCommand
{
public:
	explicit CmdNodeMove( QSharedPointer<NodeItem> pNodeItem, int pUndoId, const QPointF &pOldVal, const QPointF &pNewVal )
		: mNodeItem( pNodeItem ), mUndoId( pUndoId ), mValOld( pOldVal ), mValNew( pNewVal )
	{
		setText( QObject::tr( "Set Node Position" ) );
	}

	virtual ~CmdNodeMove( void )
	{

	}

	virtual int id( void ) const
	{
		return( mUndoId );
	}

	virtual bool mergeWith( const QUndoCommand *pCmd )
	{
		if( pCmd->id() != id() )
		{
			return( false );
		}

		const CmdNodeMove	*Cmd = dynamic_cast<const CmdNodeMove *>( pCmd );

		if( Cmd == 0 )
		{
			return( false );
		}

		QSharedPointer<NodeItem>	N1 = mNodeItem.toStrongRef();
		QSharedPointer<NodeItem>	N2 = Cmd->mNodeItem.toStrongRef();

		if( !N1 || !N2 || N1 != N2 )
		{
			return( false );
		}

		mValNew = Cmd->mValNew;

		return( true );
	}

	virtual void undo( void )
	{
		setNodePos( mValOld );
	}

	virtual void redo( void )
	{
		setNodePos( mValNew );
	}

private:
	void setNodePos( const QPointF &pPos ) const
	{
		QSharedPointer<NodeItem>	NI = mNodeItem.toStrongRef();

		if( NI )
		{
			NI->setFlags( NI->flags() & ~QGraphicsItem::ItemSendsGeometryChanges );

			NI->setNodePos( pPos );

			NI->setFlags( NI->flags() | QGraphicsItem::ItemSendsGeometryChanges );
		}
	}

private:
	QWeakPointer<NodeItem>		 mNodeItem;
	int							 mUndoId;
	QPointF						 mValOld, mValNew;
};

#endif // CMDNODEMOVE_H
