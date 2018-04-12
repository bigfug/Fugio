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
	explicit CmdNodeAdd( ContextWidgetPrivate *pContextWidget, const QString &pName, const QUuid &pUuid, const QUuid &pSrcPinUuid = QUuid(), const QUuid &pDstPinUuid = QUuid() )
		: mContextWidget( pContextWidget ), mName( pName ), mControlId( pUuid ), mExtSrcPinUuid( pSrcPinUuid ), mExtDstPinUuid( pDstPinUuid )
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
		if( !mExtSrcPinUuid.isNull() && !mIntDstPinUuid.isNull() )
		{
			mContext->disconnectPins( mExtSrcPinUuid, mIntDstPinUuid );
		}

		if( !mExtDstPinUuid.isNull() && !mIntSrcPinUuid.isNull() )
		{
			mContext->disconnectPins( mIntSrcPinUuid, mExtDstPinUuid );
		}

		if( !mExtSrcPinUuid.isNull() && !mExtDstPinUuid.isNull() )
		{
			mContext->connectPins( mExtSrcPinUuid, mExtDstPinUuid );
		}

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

		QSharedPointer<fugio::NodeInterface>	Node = mContext->createNode( mName, mNodeId, mControlId );

		if( Node && Node->control().isNull() )
		{
			Node.clear();
		}

		if( Node )
		{
			mContextWidget->saveRecovery();

			mContext->registerNode( Node );

			if( !mExtSrcPinUuid.isNull() && !mExtDstPinUuid.isNull() )
			{
				QList< QSharedPointer<fugio::PinInterface> >	InpPinLst = Node->enumInputPins();
				QList< QSharedPointer<fugio::PinInterface> >	OutPinLst = Node->enumOutputPins();

				if( !InpPinLst.isEmpty() )
				{
					mIntDstPinUuid = InpPinLst.first()->globalId();
				}

				if( !OutPinLst.isEmpty() )
				{
					mIntSrcPinUuid = OutPinLst.first()->globalId();
				}

				if( !mIntDstPinUuid.isNull() && !mIntSrcPinUuid.isNull() )
				{
					mContext->disconnectPins( mExtSrcPinUuid, mExtDstPinUuid );

					mContext->connectPins( mExtSrcPinUuid, mIntDstPinUuid );

					mContext->connectPins( mIntSrcPinUuid, mExtDstPinUuid );
				}
			}
		}
	}

private:
	ContextWidgetPrivate					*mContextWidget;
	QSharedPointer<fugio::ContextInterface>	 mContext;
	QString									 mName;
	QUuid									 mNodeId;
	QUuid									 mControlId;
	const QUuid								 mExtSrcPinUuid;
	QUuid									 mIntDstPinUuid;
	QUuid									 mIntSrcPinUuid;
	const QUuid								 mExtDstPinUuid;
};

#endif // CMDNODEADD_H
