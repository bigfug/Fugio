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
		: mContextWidget( pContextWidget ), mName( pName ), mUuid( pUuid )
	{
		setText( QObject::tr( "Add Node" ) );

		mContext = mContextWidget->context();

		App::incrementStatistic( "node-added" );

		App::recordData( "node-add",fugio::utils::uuid2string( mUuid ) );
	}

	virtual ~CmdNodeAdd( void )
	{

	}

	virtual void undo( void )
	{
		if( mNode )
		{
			mContext->unregisterNode( mNode->uuid() );
		}
	}

	virtual void redo( void )
	{
		if( mNode.isNull() )
		{
			mContextWidget->saveRecovery();
			
			mNode = mContext->createNode( mName, mUuid );

			if( mNode )
			{
				if( mNode->control().isNull() )
				{
					mNode.clear();
				}
				else
				{
					mContext->addDeferredNode( mNode );

					mContext->nodeInitialised();
				}
			}

			mContextWidget->saveRecovery();
		}

		if( mNode )
		{
			mContext->registerNode( mNode, mUuid );
		}
	}

private:
	ContextWidgetPrivate					*mContextWidget;
	QSharedPointer<fugio::ContextInterface>	 mContext;
	QSharedPointer<fugio::NodeInterface>	 mNode;
	QString									 mName;
	QUuid									 mUuid;
};

#endif // CMDNODEADD_H
