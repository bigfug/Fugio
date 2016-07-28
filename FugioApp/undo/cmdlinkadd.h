#ifndef CMDLINKADD_H
#define CMDLINKADD_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"

#include "contextwidgetprivate.h"

class CmdLinkAdd : public QUndoCommand
{
public:
	explicit CmdLinkAdd( ContextWidgetPrivate *pContextWidget, QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst )
		: mContextWidget( pContextWidget ), mPinSrc( pPinSrc ), mPinDst( pPinDst )
	{
		setText( QObject::tr( "Add Link" ) );

		mContext = mContextWidget->context();

		gApp->incrementStatistic( "link-added" );
	}

	virtual ~CmdLinkAdd( void )
	{

	}

	virtual void undo( void )
	{
		if( !mLastName.isEmpty() )
		{
			mPinDst->setName( mLastName );

			if( mPinPaired )
			{
				mPinPaired->setName( mLastName );
			}
		}

		mContext->disconnectPins( mPinSrc->globalId(), mPinDst->globalId() );

		if( mPinDst->updatable() )
		{
			mContext->pinUpdated( mPinDst );
		}
	}

	virtual void redo( void )
	{
		mContextWidget->saveRecovery();

		mContext->connectPins( mPinSrc->globalId(), mPinDst->globalId() );

		if( mPinDst->autoRename() && mPinDst->name() != mPinSrc->name() )
		{
			mLastName = mPinDst->name();

			mPinDst->setName( mPinSrc->name() );

			fugio::NodeInterface		*NI = mPinDst->node();

			if( NI )
			{
				for( fugio::NodeInterface::UuidPair UP : NI->pairedPins() )
				{
					if( mPinDst->localId() == UP.first )
					{
						mPinPaired = NI->findPinByLocalId( UP.second );
					}
				}
			}

			if( mPinPaired )
			{
				mPinPaired->setName( mPinSrc->name() );
			}
		}

		if( mPinDst->updatable() )
		{
			mContext->pinUpdated( mPinDst );
		}

		mContextWidget->saveRecovery();
	}

private:
	ContextWidgetPrivate					*mContextWidget;
	QSharedPointer<fugio::ContextInterface>	 mContext;
	QSharedPointer<fugio::PinInterface>		 mPinSrc;
	QSharedPointer<fugio::PinInterface>		 mPinDst;
	QSharedPointer<fugio::PinInterface>		 mPinPaired;
	QString									 mLastName;
};

#endif // CMDLINKADD_H
