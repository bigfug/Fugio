#ifndef CMDCONTEXTVIEWPASTE_H
#define CMDCONTEXTVIEWPASTE_H

#include <QUndoCommand>
#include <QTemporaryFile>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"
#include "contextview.h"

class CmdContextViewPaste : public QUndoCommand
{
public:
	explicit CmdContextViewPaste( ContextView *pContext, const QByteArray &pPasteData, const QPointF &pPastePoint = QPointF() )
		: mContext( pContext ), mPasteData( pPasteData ), mPastePoint( pPastePoint )
	{
		setText( QObject::tr( "Paste" ) );
	}

	virtual ~CmdContextViewPaste( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<fugio::ContextInterface>	Context = mContext->context();

		QList<QUuid>	UniqNode;

		for( QUuid U : mPasteNodes.values() )
		{
			if( UniqNode.contains( U ) )
			{
				continue;
			}

			UniqNode << U;
		}

		for( QUuid U : UniqNode )
		{
			QSharedPointer<NodeItem> NI = mContext->findNodeItem( U );

			if( NI && !NI->isGroup() )
			{
				QSharedPointer<fugio::NodeInterface> NO = Context->findNode( NI->id() );

				if( NO )
				{
					for( auto P : NO->enumInputPins() )
					{
						Context->disconnectPin( P->globalId() );
					}

					for( auto P : NO->enumOutputPins() )
					{
						Context->disconnectPin( P->globalId() );
					}
				}
			}
		}

		for( QUuid U : UniqNode )
		{
			QSharedPointer<NodeItem> NI = mContext->findNodeItem( U );

			if( NI && !NI->isGroup() )
			{
				mContext->context()->unregisterNode( U );
			}
		}

		for( QUuid U : UniqNode )
		{
			QSharedPointer<NodeItem> NI = mContext->findNodeItem( U );

			if( NI && NI->isGroup() )
			{
				mContext->groupRemove( U );
			}
		}

		for( QUuid U : mPasteNotes )
		{
			mContext->noteRemove( U );
		}

		mContext->decreasePasteOffset();
	}

	virtual void redo( void )
	{
		QTemporaryFile		TempFile;

		if( TempFile.open() )
		{
			TempFile.write( mPasteData );

			TempFile.close();
		}

		mContext->scene()->clearSelection();

		mContext->setUndoNodeUpdates( false );

		mContext->setPastePoint( mPastePoint );

		mContext->context()->load( TempFile.fileName(), true );

		mContext->setUndoNodeUpdates( true );

		mPasteNodes = mContext->loadNodeIds();
		mPasteNotes = mContext->loadNoteIds();
		mPastePins  = mContext->loadPinIds();

		mContext->clearLoadRecord();

		mContext->increasePasteOffset();
	}

private:
	ContextView				*mContext;
	const QByteArray		 mPasteData;
	QMap<QUuid,QUuid>		 mPasteNodes;
	QMap<QUuid,QUuid>		 mPasteNotes;
	QMap<QUuid,QUuid>		 mPastePins;
	const QPointF			 mPastePoint;
};

#endif // CMDCONTEXTVIEWPASTE_H

