#ifndef CMDNODEADD_H
#define CMDNODEADD_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

#include "app.h"
#include "contextprivate.h"
#include "contextwidgetprivate.h"
#include <fugio/utils.h>

class CmdNodeAdd : public QUndoCommand
{
public:
	explicit CmdNodeAdd( ContextWidgetPrivate *pContextWidget, const QString &pName, const QUuid &pUuid )
		: mContextWidget( pContextWidget ), mName( pName ), mControlId( pUuid )
	{
		setText( QObject::tr( "Add Node" ) );

		mContext = mContextWidget->context();

		gApp->incrementStatistic( "node-added" );

		gApp->recordData( "node-add", fugio::utils::uuid2string( mControlId ) );
	}

	virtual ~CmdNodeAdd( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<fugio::NodeInterface>	Node = mContext->findNode( mNodeId );

		if( Node )
		{
			mContext->unregisterNode( Node->uuid() );
		}
	}

	virtual void redo( void )
	{
		mContextWidget->saveRecovery();

		if( mNodeId.isNull() )
		{
			mNodeId = QUuid::createUuid();
		}

		QSharedPointer<fugio::NodeInterface>	Node = mContext->global()->createNode( mName, mNodeId, mControlId );

		if( Node && Node->control().isNull() )
		{
			Node.clear();
		}

		if( Node )
		{
			mContextWidget->saveRecovery();

			mContext->registerNode( Node );
		}
	}

private:
	ContextWidgetPrivate					*mContextWidget;
	QSharedPointer<fugio::ContextInterface>	 mContext;
	QString									 mName;
	QUuid									 mNodeId;
	QUuid									 mControlId;
};

#endif // CMDNODEADD_H
