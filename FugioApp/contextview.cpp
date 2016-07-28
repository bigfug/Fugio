#include "contextview.h"

#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QMainWindow>
#include <QGraphicsTextItem>
#include <QSettings>
#include <QMimeData>
#include <QGestureEvent>
#include <QScrollBar>
#include <QInputDialog>

#include <fugio/pin_interface.h>
#include <fugio/node_control_interface.h>

#include <fugio/utils.h>

#include "app.h"
#include "mainwindow.h"
#include "linkitem.h"
#include "contextwidgetprivate.h"
#include "noteitem.h"

#include "undo/cmdremove.h"
#include "undo/cmdnodeadd.h"
#include "undo/cmdsetcolour.h"
#include "undo/cmdcontextviewpaste.h"
#include "undo/cmdcontextviewcut.h"
#include "undo/cmdmove.h"
#include "undo/cmdgroup.h"
#include "undo/cmdungroup.h"
#include "undo/cmdgrouppush.h"
#include "undo/cmdgrouppop.h"

#include "nodenamedialog.h"

#include "wizards/nodeeditorwizard.h"

bool ContextView::mShownWizard = false;

ContextView::ContextView( QWidget *pParent ) :
	QGraphicsView( pParent ), mContext( 0 ), mChanged( false ), mNodePositionFlag( false ), mSaveOnlySelected( false ), mUndoNodeUpdates( true ), mNodeMoveUndoId( 0 )
{
	setScene( &mContextScene );

	setViewportUpdateMode( QGraphicsView::FullViewportUpdate );

	setDragMode( QGraphicsView::RubberBandDrag );

	setAcceptDrops( true );

	grabGesture( Qt::PinchGesture );
	//grabGesture( Qt::PanGesture );

	//connect( this, SIGNAL(rubberBandChanged(QRect,QPointF,QPointF)), this, SLOT(rubberBandChanged(QRect,QPointF,QPointF)) );

	setNoteColour( QColor( "#FCF0AD" ) );
	setNoteFontFace( "Times" );
	setNoteFontSize( 15 );

	mLabelBrush = QBrush( Qt::white );

	resetPasteOffset();
}

ContextView::~ContextView( void )
{
	if( QSharedPointer<ContextPrivate> C = qSharedPointerCast<ContextPrivate>( mContext ) )
	{
		C->clear();
	}

	mContext->global()->delContext( mContext );

	clearContext();
}

void ContextView::setContext( QSharedPointer<fugio::ContextInterface> pContext )
{
	clearContext();

	mContext = pContext;

	if( QSharedPointer<ContextPrivate> C = qSharedPointerCast<ContextPrivate>( mContext ) )
	{
		connect( C.data(), SIGNAL(loadStart(QSettings&,bool)), this, SLOT(loadStarted(QSettings&,bool)) );
		connect( C.data(), SIGNAL(loading(QSettings&,bool)), this, SLOT(loadContext(QSettings&,bool)) );
		connect( C.data(), SIGNAL(loadEnd(QSettings&,bool)), this, SLOT(loadEnded(QSettings&,bool)) );

		connect( C.data(), SIGNAL(saving(QSettings&)), this, SLOT(saveContext(QSettings&)) );

		connect( C.data(), SIGNAL(clearContext()), this, SLOT(clearContext()) );

		connect( C.data(), SIGNAL(nodeAdded(QUuid,QUuid)), this, SLOT(nodeAdded(QUuid,QUuid)) );
		connect( C.data(), SIGNAL(nodeRemoved(QUuid)), this, SLOT(nodeRemoved(QUuid)) );
		connect( C.data(), SIGNAL(nodeRenamed(QUuid,QUuid)), this, SLOT(nodeRenamed(QUuid,QUuid)) );
		connect( C.data(), SIGNAL(nodeUpdated(QUuid)), this, SLOT(nodeChanged(QUuid)) );
		connect( C.data(), SIGNAL(nodeActivated(QUuid)), this, SLOT(nodeActivated(QUuid)) );

		connect( C.data(), SIGNAL(pinAdded(QUuid,QUuid)), this, SLOT(pinAdded(QUuid,QUuid)) );
		connect( C.data(), SIGNAL(pinRenamed(QUuid,QUuid,QUuid)), this, SLOT(pinRenamed(QUuid,QUuid,QUuid)) );
		connect( C.data(), SIGNAL(pinRemoved(QUuid,QUuid)), this, SLOT(pinRemoved(QUuid,QUuid)) );

		connect( C.data(), SIGNAL(linkAdded(QUuid,QUuid)), this, SLOT(linkAdded(QUuid,QUuid)) );
		connect( C.data(), SIGNAL(linkRemoved(QUuid,QUuid)), this, SLOT(linkRemoved(QUuid,QUuid)) );
	}

#if defined( USE_CONTEXT_MODEL )
	mContextModel.setContext( pContext );
#endif
}

ContextWidgetPrivate *ContextView::widget( void )
{
	QObject						*O = this;
	ContextWidgetPrivate		*C = 0;

	while( O != 0 && ( C = qobject_cast<ContextWidgetPrivate *>( O ) ) == 0 )
	{
		O = O->parent();
	}

	return( C );
}

void ContextView::sortSelectedItems( QList<NodeItem *> &pNodeList, QList<NodeItem *> &pGroupList, QList<LinkItem *> &pLinkList, QList<NoteItem *> &pNoteList, bool pRecurse )
{
	for( QGraphicsItem *Item : scene()->selectedItems() )
	{
		if( NodeItem *NI = qgraphicsitem_cast<NodeItem *>( Item ) )
		{
			if( NI->groupId() == groupId() )
			{
				if( !NI->isGroup() )
				{
					pNodeList.append( NI );
				}
				else
				{
					pGroupList.append( NI );
				}
			}

			continue;
		}

		if( LinkItem *LI = qgraphicsitem_cast<LinkItem *>( Item ) )
		{
			pLinkList.append( LI );

			continue;
		}

		if( NoteItem *TI = qgraphicsitem_cast<NoteItem *>( Item ) )
		{
			if( TI->groupId() == groupId() )
			{
				pNoteList.append( TI );
			}

			continue;
		}
	}

	// Check if we have selected groups - we need to select their contents if so

	if( pRecurse && !pGroupList.isEmpty() )
	{
		QList<QUuid>		GrpIds = recursiveGroupIds( pGroupList );

		pGroupList = nodesFromIds( GrpIds );

		// We need to select all the nodes that may have just been added in groups

		GrpIds = nodeItemIds( pGroupList );

		itemsInGroups( GrpIds, pNodeList );

		notesInGroups( GrpIds, pNoteList );
	}
}

bool ContextView::itemsForRemoval( QList<NodeItem *> &pNodeItemList, QList<NodeItem *> &pGroupItemList, QList<LinkItem *> &pLinkItemList, QList<NoteItem *> &pNoteItemList, QList<QSharedPointer<fugio::NodeInterface> > &pNodeList, QList<QSharedPointer<NodeItem>> &pGroupList, QMultiMap<QUuid, QUuid> &pLinkList, QList<QSharedPointer<NoteItem> > &pNoteList )
{
	sortSelectedItems( pNodeItemList, pGroupItemList, pLinkItemList, pNoteItemList, true );

	if( pNodeItemList.isEmpty() && pGroupItemList.isEmpty() && pLinkItemList.isEmpty() && pNoteItemList.isEmpty() )
	{
		return( false );
	}

	for( NodeItem *Node : pNodeItemList )
	{
		pNodeList.append( mContext->findNode( Node->id() ) );
	}

	for( NodeItem *GI : pGroupItemList )
	{
		pGroupList.append( mNodeList.value( GI->id() ) );
	}

	for( LinkItem *Link : pLinkItemList )
	{
		pLinkList.insert( Link->srcPin()->pin()->globalId(), Link->dstPin()->pin()->globalId() );
	}

	for( NoteItem *Note : pNoteItemList )
	{
		for( QSharedPointer<NoteItem> P : mNoteList )
		{
			if( P.data() != Note )
			{
				continue;
			}

			pNoteList.append( P );
		}
	}

	return( true );
}

//bool ContextView::itemsForRemoval( QList<QSharedPointer<fugio::NodeInterface> > &pNodeList, QMultiMap<QUuid, QUuid> &pLinkList, QList<QSharedPointer<NodeItem>> &pGroupList, QList<QSharedPointer<NoteItem> > &pNoteList )
//{
//	return( itemsForRemoval( mNodeItemList, mGroupItemList, mLinkItemList, mNoteItemList, pNodeList, pGroupList, pLinkList, pNoteList ) );
//}

QList<QUuid> ContextView::nodeItemIds( const QList<NodeItem *> &pNodeList )
{
	QList<QUuid>		IdsLst;

	for( NodeItem *NI : pNodeList )
	{
		IdsLst << NI->id();
	}

	return( IdsLst );
}

QList<QUuid> ContextView::noteItemIds( const QList<NoteItem *> &pNoteList )
{
	QList<QUuid>		IdsLst;

	for( NoteItem *NI : pNoteList )
	{
		IdsLst << NI->id();
	}

	return( IdsLst );
}

QList<NodeItem *> ContextView::nodesFromIds( const QList<QUuid> &pIdsLst ) const
{
	QList<NodeItem *>		NodeList;

	for( const QUuid &NID : pIdsLst )
	{
		QSharedPointer<NodeItem>		NI = mNodeList.value( NID );

		if( NI )
		{
			NodeList << NI.data();
		}
	}

	return( NodeList );
}

QList<QUuid> ContextView::recursiveGroupIds( const QList<NodeItem *> &pGroupList ) const
{
	QList<QUuid>		IdsLst;

	for( NodeItem *NI : pGroupList )
	{
		if( !IdsLst.contains( NI->id() ) )
		{
			IdsLst << NI->id();
		}
	}

	QList<QUuid>		GrpLst;

	do
	{
		IdsLst.append( GrpLst );

		GrpLst.clear();

		for( QMap<QUuid, QSharedPointer<NodeItem> >::const_iterator it = mNodeList.cbegin() ; it != mNodeList.cend() ; it++ )
		{
			if( !IdsLst.contains( it.key() ) )
			{
				if( !GrpLst.contains( it.key() ) && IdsLst.contains( it.value()->groupId() ) && it.value()->isGroup() )
				{
					GrpLst << it.key();
				}
			}
		}

	} while( !GrpLst.isEmpty() );

	return( IdsLst );
}

void ContextView::itemsInGroups( const QList<QUuid> &pGroupIdList, QList<NodeItem *> &pNodeList ) const
{
	for( QMap<QUuid, QSharedPointer<NodeItem> >::const_iterator it = mNodeList.cbegin() ; it != mNodeList.cend() ; it++ )
	{
		if( pGroupIdList.contains( it.value()->groupId() ) )
		{
			pNodeList << it.value().data();
		}
	}
}

void ContextView::notesInGroups( const QList<QUuid> &pGroupIdList, QList<NoteItem *> &pNoteList ) const
{
	for( QSharedPointer<NoteItem> NI : mNoteList )
	{
		if( pGroupIdList.contains( NI->groupId() ) )
		{
			pNoteList << NI.data();
		}
	}
}

void ContextView::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
	if( scene()->selectedItems().size() == 1 )
	{
		NodeItem		*NI = qgraphicsitem_cast<NodeItem *>( scene()->selectedItems().first() );

		if( NI )
		{
			if( mGroupIds.contains( NI->id() ) )
			{
				CmdGroupPush	*CMD = new CmdGroupPush( this, NI->id() );

				if( CMD )
				{
					widget()->undoStack()->push( CMD );
				}
			}

			pEvent->accept();

			return;
		}
	}

//	emit nodeInspection( 0 );

	if( !itemAt( pEvent->pos() ) )
	{
		NodeNameDialog		NodeDiag( this );

		if( NodeDiag.exec() == QDialog::Accepted )
		{
			CmdNodeAdd		*Cmd = new CmdNodeAdd( widget(), gApp->global().nodeMap().value( NodeDiag.nodename() ).mName, NodeDiag.nodename() );

			setNodePositionFlag();

			mNodePosition = mapToScene( pEvent->pos() );

			resetPasteOffset();

			widget()->undoStack()->push( Cmd );
		}

		pEvent->accept();

		return;
	}

	QGraphicsView::mouseDoubleClickEvent( pEvent );
}

void ContextView::keyReleaseEvent( QKeyEvent *pEvent )
{
	// Only process delete/backspace if there is no item that currently has focus (such as notes/editors)

	if( !scene()->focusItem() && ( pEvent->key() == Qt::Key_Delete || pEvent->key() == Qt::Key_Backspace ) )
	{
		QList<NodeItem *>								NodeItemList;
		QList<NodeItem *>								GroupItemList;
		QList<LinkItem *>								LinkItemList;
		QList<NoteItem *>								NoteItemList;

		QList< QSharedPointer<fugio::NodeInterface> >	NodeList;
		QList<QSharedPointer<NodeItem>>					GroupList;
		QMultiMap<QUuid,QUuid>							LinkList;
		QList<QSharedPointer<NoteItem>>					NoteList;

		if( itemsForRemoval( NodeItemList, GroupItemList, LinkItemList, NoteItemList, NodeList, GroupList, LinkList, NoteList ) )
		{
			QStringList		StuckNodes;

			for( NodeItem *NI : GroupItemList )
			{
				if( NI->hasPinsInGroup() )
				{
					StuckNodes << NI->name();
				}
			}

			if( !StuckNodes.isEmpty() )
			{
				QMessageBox::warning( this, tr( "Cannot delete" ), tr( "Can't delete the following nodes because they have pins being used by their parent group:\n\n%1" ).arg( StuckNodes.join( "\n" ) ), QMessageBox::Cancel, QMessageBox::NoButton );

				return;
			}

			QMap<QUuid,QUuid>								NodeGroups = nodeGroups( NodeItemList );

			CmdRemove		*Cmd = new CmdRemove( this, NodeList, GroupList, LinkList, NoteList, NodeGroups );

			if( Cmd )
			{
				widget()->undoStack()->push( Cmd );
			}
		}

		pEvent->accept();
	}
	else
	{
		QWidget::keyReleaseEvent( pEvent );
	}
}

//-----------------------------------------------------------------------------
// LOAD AND SAVE

void ContextView::loadStarted( QSettings &pSettings, bool pPartial )
{
	Q_UNUSED( pPartial )

	mNodeOrig.clear();

	if( pSettings.childGroups().contains( "positions" ) )
	{
		pSettings.beginGroup( "positions" );

		for( const QString &NodeName : pSettings.childKeys() )
		{
			QUuid	NodeUuid = fugio::utils::string2uuid( NodeName );

			if( NodeUuid.isNull() )
			{
				continue;
			}

			QVariant	PntSrc = pSettings.value( NodeName );
			QPointF		PntDat;

			if( PntSrc.type() == QVariant::PointF )
			{
				PntDat = PntSrc.toPointF();
			}
			else if( PntSrc.type() == QVariant::String )
			{
				PntDat = fugio::utils::string2point( PntSrc.toString() );
			}

			mPastePositions.insert( NodeUuid, PntDat );
		}

		pSettings.endGroup();
	}
}

void ContextView::loadContext( QSettings &pSettings, bool pPartial )
{
	//-------------------------------------------------------------------------
	// Positions

	if( pSettings.childGroups().contains( "positions" ) )
	{
		pSettings.beginGroup( "positions" );

		for( const QString &NodeName : pSettings.childKeys() )
		{
			QUuid	NodeUuid = fugio::utils::string2uuid( NodeName );

			if( NodeUuid.isNull() )
			{
				continue;
			}

			QVariant	PntSrc = pSettings.value( NodeName );
			QPointF		PntDat;

			if( PntSrc.type() == QVariant::PointF )
			{
				PntDat = PntSrc.toPointF();
			}
			else if( PntSrc.type() == QVariant::String )
			{
				PntDat = fugio::utils::string2point( PntSrc.toString() );
			}

			mPositions.insert( NodeUuid, PntDat );

			if( QSharedPointer<NodeItem> N = mNodeList.value( NodeUuid ) )
			{
				N->setNodePosSilent( PntDat );
			}
		}

		pSettings.endGroup();
	}

	for( auto it = mNodeList.begin() ; it != mNodeList.end() ; it++ )
	{
		it.value()->updateActiveState();
	}

	//-------------------------------------------------------------------------
	// Groups

	QMap<QUuid,QUuid>		GroupIdMap;

	if( pSettings.childGroups().contains( "groups" ) )
	{
		pSettings.beginGroup( "groups" );

		const int	GroupsVersion = pSettings.value( "version", 2 ).toInt();

		if( GroupsVersion == 2 )
		{
			for( const QString &KS : pSettings.childKeys() )
			{
				const QUuid	K = fugio::utils::string2uuid( KS );

				if( K.isNull() )
				{
					continue;
				}

				if( !mGroupIds.contains( K ) )
				{
					mGroupIds << K;
				}

				if( !mNodeList.contains( K ) )
				{
					nodeAdded( K, K );
				}

				if( QSharedPointer<NodeItem> NI = mNodeList.value( K ) )
				{
					NI->setName( pSettings.value( KS ).toString() );

					NI->setIsGroup( true );

					QPointF	NP = mPositions.value( NI->id(), NI->pos() );

					processGroupLinks( NI );

					NI->setNodePos( NP );
				}
			}

			pSettings.endGroup();
		}
		else if( GroupsVersion == 3 )
		{
			pSettings.endGroup();

			int		GroupCount = pSettings.beginReadArray( "groups" );

			for( int i = 0 ; i < GroupCount ; i++ )
			{
				pSettings.setArrayIndex( i );

				const QString		GroupName = pSettings.value( "name" ).toString();
				const QUuid			OrigId    = fugio::utils::string2uuid( pSettings.value( "uuid" ).toString() );
				const QUuid			NodeId    = ( mNodeList.contains( OrigId ) ? QUuid::createUuid() : OrigId );

				//const QTransform	Transform = pSettings.value( "transform" ).value<QTransform>();

				if( NodeId.isNull() )
				{
					continue;
				}

				GroupIdMap.insert( OrigId, NodeId );

				if( !mGroupIds.contains( NodeId ) )
				{
					mGroupIds << NodeId;
				}

				if( !mNodeList.contains( NodeId ) )
				{
					nodeAdded( NodeId, OrigId );
				}

				if( QSharedPointer<NodeItem> NI = mNodeList.value( NodeId ) )
				{
					NI->setName( GroupName );

					NI->setIsGroup( true );

					QPointF	NP = mPositions.value( NI->id(), NI->pos() );

					processGroupLinks( NI );

					NI->setNodePos( NP );
				}
			}

			pSettings.endArray();
		}
	}

	//-------------------------------------------------------------------------
	// Notes

	if( pSettings.childGroups().contains( "notes" ) )
	{
		int		NoteCount = pSettings.beginReadArray( "notes" );

		for( int i = 0 ; i < NoteCount ; i++ )
		{
			pSettings.setArrayIndex( i );

			QString		Text = pSettings.value( "note" ).toString();
			QColor		Col  = pSettings.value( "colour", noteColour() ).value<QColor>();
			QUuid		Id   = fugio::utils::string2uuid( pSettings.value( "uuid", fugio::utils::uuid2string( QUuid::createUuid() ) ).toString() );
			QUuid		GroupId = fugio::utils::string2uuid( pSettings.value( "group", fugio::utils::uuid2string( QUuid() ) ).toString() );

			if( GroupIdMap.contains( GroupId ) )
			{
				GroupId = GroupIdMap.value( GroupId );
			}

			for( QSharedPointer<NoteItem> NI : mNoteList )
			{
				if( NI->id() == Id )
				{
					Id = QUuid::createUuid();
				}
			}

			QPointF		Pos  = pSettings.value( "position" ).toPointF();

			if( pSettings.value( "position" ).type() == QVariant::PointF )
			{
				Pos  = pSettings.value( "position" ).toPointF();
			}
			else if( pSettings.value( "position" ).type() == QVariant::String )
			{
				Pos = fugio::utils::string2point( pSettings.value( "position" ).toString() );
			}

			QSharedPointer<NoteItem>	Note = noteAdd( Text, Id );

			Note->setPos( Pos );
			Note->setBackgroundColour( Col );
			Note->setGroupId( GroupId );

			if( mPasteOffset > 0 )
			{
				Note->moveBy( mPasteOffset, mPasteOffset );
			}
		}

		pSettings.endArray();
	}

	//-------------------------------------------------------------------------
	// Node Groups

	if( pSettings.childGroups().contains( "node-groups" ) )
	{
		pSettings.beginGroup( "node-groups" );

		for( const QString &KS : pSettings.childKeys() )
		{
			QUuid	KID = fugio::utils::string2uuid( KS );

			if( KID.isNull() )
			{
				continue;
			}

			if( mNodeOrig.contains( KID ) )
			{
				KID = mNodeOrig.value( KID );
			}

			QUuid	GID = fugio::utils::string2uuid( pSettings.value( KS ).toString() );

			if( GroupIdMap.contains( GID ) )
			{
				GID = GroupIdMap.value( GID );
			}

			if( QSharedPointer<NodeItem> NI = mNodeList.value( KID ) )
			{
				NI->setGroupId( GID );
			}
			else
			{
				NI->setGroupId( groupId() );
			}
		}

		pSettings.endGroup();
	}

	//-------------------------------------------------------------------------
	// Colours

	if( pSettings.childGroups().contains( "colours" ) )
	{
		pSettings.beginGroup( "colours" );

		for( const QString &KS : pSettings.childKeys() )
		{
			const QUuid	K = fugio::utils::string2uuid( KS );

			if( K.isNull() )
			{
				continue;
			}

			if( QSharedPointer<NodeItem> NI = mNodeList.value( K ) )
			{
				pSettings.beginGroup( KS );

				for( const QString &KP : pSettings.childKeys() )
				{
					QUuid	K = fugio::utils::string2uuid( KP );

					if( K.isNull() )
					{
						continue;
					}

					if( mNodeOrig.contains( K ) )
					{
						K = mNodeOrig.value( K );
					}

					if( PinItem	*PI = NI->findPinOutput( fugio::utils::string2uuid( KP ) ) )
					{
						PI->setColour( pSettings.value( KP ).value<QColor>() );
					}
				}

				pSettings.endGroup();
			}
		}

		pSettings.endGroup();
	}

	updateItemVisibility();

	//-------------------------------------------------------------------------

#if !defined( Q_OS_RASPERRY_PI )
	if( !pPartial )
	{
		gApp->mainWindow()->restoreGeometry( pSettings.value( "mainwindow/geometry", gApp->mainWindow()->saveGeometry() ).toByteArray() );
		gApp->mainWindow()->restoreState( pSettings.value( "mainwindow/state", gApp->mainWindow()->saveState() ).toByteArray() );
	}
#endif

	if( !pPartial )
	{
		scene()->clearSelection();
	}
}

void ContextView::loadEnded( QSettings &pSettings, bool pPartial )
{
	Q_UNUSED( pSettings )
	Q_UNUSED( pPartial )

	mPastePositions.clear();

	mNodeOrig.clear();
}

void ContextView::saveContext( QSettings &pSettings ) const
{
	int			ArrayIndex;

	//-------------------------------------------------------------------------
	// Nodes

	pSettings.beginGroup( "positions" );

	pSettings.setValue( "version", 2 );

	for( auto Uuid : mNodeList.keys() )
	{
		QSharedPointer<NodeItem>	Node = mNodeList[ Uuid ];

		if( mSaveOnlySelected && !mNodeItemList.contains( Node.data() ) && !mGroupItemList.contains( Node.data() ) )
		{
			continue;
		}

		pSettings.setValue( fugio::utils::uuid2string( Node->id() ), fugio::utils::point2string( Node->pos() ) );
	}

	pSettings.endGroup();

	//-------------------------------------------------------------------------
	// Notes

	pSettings.beginWriteArray( "notes" );

	pSettings.setValue( "version", 2 );

	ArrayIndex = 0;

	for( int i = 0 ; i < mNoteList.size() ; i++ )
	{
		QSharedPointer<NoteItem>	Note = mNoteList.at( i );

		if( mSaveOnlySelected && !mNoteItemList.contains( Note.data() ) )
		{
			continue;
		}

		pSettings.setArrayIndex( ArrayIndex++ );

		pSettings.setValue( "note", Note->toPlainText() );
		pSettings.setValue( "position", fugio::utils::point2string( Note->pos() ) );
		pSettings.setValue( "uuid", fugio::utils::uuid2string( Note->id() ) );

		if( !Note->groupId().isNull() )
		{
			QSharedPointer<NodeItem>	Node = mNodeList[ Note->groupId() ];

			if( Node )
			{
				if( !mSaveOnlySelected || mGroupItemList.contains( Node.data() ) )
				{
					pSettings.setValue( "group", fugio::utils::uuid2string( Note->groupId() ) );
				}
			}
		}

		if( Note->backgroundColour() != noteColour() )
		{
			pSettings.setValue( "colour", Note->backgroundColour() );
		}
	}

	pSettings.endArray();

	//-------------------------------------------------------------------------
	// Groups

	pSettings.beginWriteArray( "groups" );

	pSettings.setValue( "version", 3 );

	ArrayIndex = 0;

	for( int i = 0 ; i < mGroupIds.size() ; i++ )
	{
		const QUuid					GID  = mGroupIds[ i ];
		QSharedPointer<NodeItem>	Node = mNodeList[ GID ];

		if( !Node || ( mSaveOnlySelected && !mGroupItemList.contains( Node.data() ) ) )
		{
			continue;
		}

		pSettings.setArrayIndex( ArrayIndex++ );

		pSettings.setValue( "name", Node->name() );
		pSettings.setValue( "uuid", fugio::utils::uuid2string( GID ) );
		pSettings.setValue( "transform", QTransform() );

		if( !Node->groupId().isNull() )
		{
			pSettings.setValue( "group", fugio::utils::uuid2string( Node->groupId() ) );
		}
	}

	pSettings.endArray();

/*
	pSettings.beginGroup( "groups" );

	pSettings.setValue( "version", 2 );

	for( QUuid GID : mGroupIds )
	{
		QSharedPointer<NodeItem>	Node = mNodeList[ GID ];

		if( mSaveOnlySelected && !Node->isSelected() )
		{
			continue;
		}

		pSettings.setValue( fugio::utils::uuid2string( GID ), Node->name() );
	}

	pSettings.endGroup();
*/

	//-------------------------------------------------------------------------
	// Node Groups

	pSettings.beginGroup( "node-groups" );

	pSettings.setValue( "version", 2 );

	for( const QUuid &K : mNodeList.keys() )
	{
		QSharedPointer<NodeItem>	Node = mNodeList[ K ];

		QUuid	G = Node->groupId();

		if( G.isNull() )
		{
			continue;
		}

		QSharedPointer<NodeItem>	Group = mNodeList[ G ];

		if( !Group )
		{
			continue;
		}

		if( mSaveOnlySelected && !mGroupItemList.contains( Group.data() ) )
		{
			continue;
		}

		if( mSaveOnlySelected && !mNodeItemList.contains( Node.data() ) && !mGroupItemList.contains( Node.data() ) )
		{
			continue;
		}

		pSettings.setValue( fugio::utils::uuid2string( K ), fugio::utils::uuid2string( G ) );
	}

	pSettings.endGroup();

	//-------------------------------------------------------------------------
	// Colours

	pSettings.beginGroup( "colours" );

	pSettings.setValue( "version", 2 );

	for( auto it = mNodeList.begin() ; it != mNodeList.end() ; it++ )
	{
		QSharedPointer<NodeItem>	NI = it.value();

		if( mSaveOnlySelected && !mNodeItemList.contains( NI.data() ) && !mGroupItemList.contains( NI.data() ) )
		{
			continue;
		}

		const QString	KS = fugio::utils::uuid2string( NI->id() );

		pSettings.setValue( KS, NI->colour() );

		pSettings.beginGroup( KS );

		for( PinItem *PI : NI->outputs() )
		{
			pSettings.setValue( fugio::utils::uuid2string( PI->uuid() ), PI->colour() );
		}

		pSettings.endGroup();
	}

	pSettings.endGroup();

	//-------------------------------------------------------------------------

	if( !mSaveOnlySelected )
	{
		pSettings.setValue( "mainwindow/geometry", gApp->mainWindow()->saveGeometry() );
		pSettings.setValue( "mainwindow/state", gApp->mainWindow()->saveState() );
	}
}

void ContextView::clearContext( void )
{
	mNodeMoveData.clear();
	mNoteMoveData.clear();

	mPositions.clear();

	mGroupIds.clear();

	mGroupStack.clear();

	m_GroupId = QUuid();

	mNoteList.clear();

	mNodeList.clear();

	if( scene() )
	{
		scene()->clear();
	}
}

//-----------------------------------------------------------------------------
// fugio::ContextSignals

void ContextView::nodeAdded( QUuid pNodeId, QUuid pOrigId )
{
	QPointF				NodePoint = mPositions.value( pNodeId, mPastePositions.value( pOrigId ) );

	if( !mNodeList.contains( pNodeId ) )
	{
		QSharedPointer<NodeItem>	NewNodeItem = QSharedPointer<NodeItem>( new NodeItem( this, pNodeId, NodePoint ) );

		if( NewNodeItem )
		{
			mNodeList.insert( pNodeId, NewNodeItem );

			NewNodeItem->setGroupId( groupId() );

			if( mNodePositionFlag )
			{
				NewNodeItem->setPos( mNodePosition );

				scene()->clearSelection();
			}
			else if( mPasteOffset > 0 )
			{
				QPoint		TmpPnt = mapFromScene( NodePoint );
				QPointF		PstOff;

				PstOff = mapToScene( TmpPnt + QPoint( mPasteOffset, mPasteOffset ) );

				NewNodeItem->setNodePosSilent( PstOff );

				mPasteNodes.append( pNodeId );
			}

			NewNodeItem->setSelected( true );
		}
	}
	else
	{
		scene()->addItem( mNodeList.value( pNodeId ).data() );
	}

	mNodePositionFlag = false;

	mNodeOrig.insert( pOrigId, pNodeId );
}

void ContextView::nodeAdd( QSharedPointer<NodeItem> Node )
{
	if( scene() )
	{
		scene()->addItem( Node.data() );
	}

	mNodeList.insert( Node->id(), Node );
}

void ContextView::nodeRemove( QSharedPointer<NodeItem> Node )
{
	if( scene() )
	{
		scene()->removeItem( Node.data() );
	}

	mNodeList.remove( Node->id() );
}

void ContextView::nodeRemoved( QUuid pNodeId )
{
//	mPositions.remove( pNodeId );

	auto	ItemIterator = mNodeList.find( pNodeId );

	if( ItemIterator == mNodeList.end() )
	{
		return;
	}

	QSharedPointer<NodeItem>	Node = ItemIterator.value();

	if( !Node )
	{
		return;
	}

	nodeRemove( Node );
}

void ContextView::nodeRenamed( QUuid pNodeId1, QUuid pNodeId2 )
{
	QMap<QUuid,QPointF>::iterator		pit = mPositions.find( pNodeId1 );

	if( pit != mPositions.end() )
	{
		QPointF		P = pit.value();

		mPositions.remove( pNodeId1 );

		mPositions.insert( pNodeId2, P );
	}

	auto	iit = mNodeList.find( pNodeId1 );

	if( iit != mNodeList.end() )
	{
		QSharedPointer<NodeItem>	N = iit.value();

		mNodeList.remove( pNodeId1 );

		mNodeList.insert( pNodeId2, N );

		N->setNodeId( pNodeId2 );

		//qDebug() << "ContextView::nodeRenamed" << pNodeId1 << pNodeId2;
	}
}

void ContextView::nodeChanged( QUuid pNodeId )
{
	auto	ItemIterator = mNodeList.find( pNodeId );

	if( ItemIterator == mNodeList.end() )
	{
		return;
	}

	QSharedPointer<NodeItem>	Node = ItemIterator.value();

	if( Node == 0 )
	{
		return;
	}

	Node->updateGui();
}

void ContextView::nodeActivated( QUuid pNodeId )
{
	auto	ItemIterator = mNodeList.find( pNodeId );

	if( ItemIterator == mNodeList.end() )
	{
		return;
	}

	QSharedPointer<NodeItem>	Node = ItemIterator.value();

	if( Node == 0 )
	{
		return;
	}

	Node->updateActiveState();
}

void ContextView::pinAdded( QUuid pNodeId, QUuid pPinId )
{
	auto	ItemIterator = mNodeList.find( pNodeId );

	if( ItemIterator == mNodeList.end() )
	{
		return;
	}

	QSharedPointer<NodeItem>	Node = ItemIterator.value();

	if( Node == 0 )
	{
		return;
	}

	QSharedPointer<fugio::PinInterface>	IntPin = mContext->findPin( pPinId );

	if( IntPin == 0 )
	{
		return;
	}

	if( IntPin->direction() == PIN_INPUT )
	{
		Node->pinInputAdd( IntPin );
	}
	else if( IntPin->direction() == PIN_OUTPUT )
	{
		Node->pinOutputAdd( IntPin );
	}

	Node->layoutPins();
}

void ContextView::pinRenamed( QUuid pNodeId, QUuid pPinId1, QUuid pPinId2 )
{
	Q_UNUSED( pNodeId )
	Q_UNUSED( pPinId1 )
	Q_UNUSED( pPinId2 )
}

void ContextView::pinRemoved( QUuid pNodeId, QUuid pPinId )
{
	auto	ItemIterator = mNodeList.find( pNodeId );

	if( ItemIterator == mNodeList.end() )
	{
		return;
	}

	QSharedPointer<NodeItem>	Node = ItemIterator.value();

	if( Node == 0 )
	{
		return;
	}

	QSharedPointer<fugio::PinInterface>	IntPin = mContext->findPin( pPinId );

	if( IntPin == 0 )
	{
		return;
	}

	if( IntPin->direction() == PIN_INPUT )
	{
		Node->pinInputRemove( IntPin );
	}
	else if( IntPin->direction() == PIN_OUTPUT )
	{
		Node->pinOutputRemove( IntPin );
	}

	Node->layoutPins();
}

void ContextView::linkAdded( QUuid pPinId1, QUuid pPinId2 )
{
	QSharedPointer<fugio::PinInterface>		 Pin1 = mContext->findPin( pPinId1 );
	QSharedPointer<fugio::PinInterface>		 Pin2 = mContext->findPin( pPinId2 );

	if( !Pin1 || !Pin2 )
	{
		return;
	}

	fugio::NodeInterface					*Node1 = Pin1->node();
	fugio::NodeInterface					*Node2 = Pin2->node();

	QSharedPointer<NodeItem>				 NodeItem1 = mNodeList.value( Node1->uuid() );
	QSharedPointer<NodeItem>				 NodeItem2 = mNodeList.value( Node2->uuid() );

	PinItem									*PinItem1 = NodeItem1->findPinInput( Pin1 );
	PinItem									*PinItem2 = NodeItem2->findPinOutput( Pin2 );

	if( !PinItem1 || !PinItem2 )
	{
		return;
	}

	if( PinItem1->hasLink( PinItem2 ) )
	{
		return;
	}

	LinkItem *RealLink = new LinkItem();

	if( RealLink )
	{
		scene()->addItem( RealLink );

		RealLink->setSrcPin( PinItem2 );
		RealLink->setDstPin( PinItem1 );

		PinItem1->linkAdd( RealLink );
		PinItem2->linkAdd( RealLink );

		QColor LnkCol = PinItem2->colour();

		if( LnkCol != RealLink->colour() )
		{
			RealLink->setColour( LnkCol );
		}

		RealLink->setSelected( true );
	}

	if( NodeItem1->groupId() != m_GroupId || NodeItem2->groupId() != m_GroupId || NodeItem1->groupId() != NodeItem2->groupId() )
	{
		RealLink->setVisible( false );
		RealLink->setSelected( false );

		if( NodeItem1->groupId() != m_GroupId )
		{
			NodeItem1 = mNodeList.value( NodeItem1->groupId() );

			PinItem1 = NodeItem1->findPinInput( Pin1 );
		}

		if( NodeItem2->groupId() != m_GroupId )
		{
			NodeItem2 = mNodeList.value( NodeItem2->groupId() );

			PinItem2 = NodeItem2->findPinOutput( Pin2 );
		}

		if( !PinItem1 || !PinItem2 )
		{
			return;
		}

		if( PinItem1->hasLink( PinItem2 ) )
		{
			return;
		}

		LinkItem *GroupLink = new LinkItem();

		if( GroupLink )
		{
			scene()->addItem( GroupLink );

			GroupLink->setSrcPin( PinItem2 );
			GroupLink->setDstPin( PinItem1 );

			PinItem1->linkAdd( GroupLink );
			PinItem2->linkAdd( GroupLink );

			QColor LnkCol = PinItem2->colour();

			if( LnkCol != GroupLink->colour() )
			{
				GroupLink->setColour( LnkCol );
			}

			GroupLink->setSelected( true );
		}
	}
}

void ContextView::linkRemoved( QUuid pPinId1, QUuid pPinId2 )
{
	QSharedPointer<fugio::PinInterface>		 Pin1 = mContext->findPin( pPinId1 );
	QSharedPointer<fugio::PinInterface>		 Pin2 = mContext->findPin( pPinId2 );

	if( !Pin1 || !Pin2 )
	{
		return;
	}

	fugio::NodeInterface					*Node1 = Pin1->node();
	fugio::NodeInterface					*Node2 = Pin2->node();

	QSharedPointer<NodeItem>				 NodeItem1 = mNodeList.value( Node1->uuid() );
	QSharedPointer<NodeItem>				 NodeItem2 = mNodeList.value( Node2->uuid() );

	if( !NodeItem1 || !NodeItem2 )
	{
		return;
	}

	PinItem									*PinItem1 = NodeItem1->findPinInput( Pin1 );
	PinItem									*PinItem2 = NodeItem2->findPinOutput( Pin2 );

	if( !PinItem1 || !PinItem2 )
	{
		PinItem1 = NodeItem2->findPinInput( Pin2 );
		PinItem2 = NodeItem1->findPinOutput( Pin1 );

		if( !PinItem1 || !PinItem2 )
		{
			return;
		}
	}

	if( LinkItem *Link = PinItem2->findLink( PinItem1 ) )
	{
		PinItem1->linkRem( Link );
		PinItem2->linkRem( Link );

		delete Link;
	}

	if( NodeItem1->groupId() != m_GroupId )
	{
		NodeItem1 = mNodeList.value( NodeItem1->groupId() );

		PinItem1 = NodeItem1->findPinInput( Pin1 );
	}

	if( NodeItem2->groupId() != m_GroupId )
	{
		NodeItem2 = mNodeList.value( NodeItem2->groupId() );

		PinItem2 = NodeItem2->findPinOutput( Pin2 );
	}

	if( !PinItem1 || !PinItem2 )
	{
		return;
	}

	if( LinkItem *Link = PinItem2->findLink( PinItem1 ) )
	{
		PinItem1->linkRem( Link );
		PinItem2->linkRem( Link );

		delete Link;
	}
}

QSharedPointer<NoteItem> ContextView::noteAdd( const QString &pText, QUuid pUuid )
{
#if defined( USE_CONTEXT_MODEL )
	QUuid						NoteUuid = mContextModel.createNote( pUuid );
#else
	QUuid						NoteUuid = ( pUuid.isNull() ? QUuid::createUuid() : pUuid );
#endif

	QSharedPointer<NoteItem>	TextItem = QSharedPointer<NoteItem>( new NoteItem( this, NoteUuid, pText ) );

	if( TextItem )
	{
		mNoteList.append( TextItem );

		scene()->addItem( TextItem.data() );

		TextItem->setSelected( true );
	}

	return( TextItem );
}

void ContextView::updateItemVisibility()
{
	for( QGraphicsItem *Item : scene()->items() )
	{
		NodeItem		*NI = qgraphicsitem_cast<NodeItem *>( Item );

		if( NI )
		{
			NI->setVisible( NI->groupId() == m_GroupId );

			continue;
		}

		LinkItem		*LI = qgraphicsitem_cast<LinkItem *>( Item );

		if( LI )
		{
			PinItem			*SrcPin = LI->srcPin();
			PinItem			*DstPin = LI->dstPin();

			if( SrcPin && DstPin )
			{
				NodeItem		*SrcNod = qgraphicsitem_cast<NodeItem *>( SrcPin->parentItem() );
				NodeItem		*DstNod = qgraphicsitem_cast<NodeItem *>( DstPin->parentItem() );

				if( SrcNod->groupId() != m_GroupId || DstNod->groupId() != m_GroupId )
				{
					LI->setVisible( false );
				}
				else
				{
					LI->setVisible( true );
				}
			}

			continue;
		}

		NoteItem		*TI = qgraphicsitem_cast<NoteItem *>( Item );

		if( TI != 0 )
		{
			TI->setVisible( TI->groupId() == m_GroupId );

			continue;
		}
	}
}

void ContextView::setGroupId( QUuid GroupId )
{
	if( m_GroupId == GroupId )
	{
		return;
	}

	m_GroupId = GroupId;

	emit groupIdChanged( GroupId );

	updateItemVisibility();
}

void ContextView::setSelectedColour( const QColor &pColor )
{
	sortSelectedItems( mNodeItemList, mGroupItemList, mLinkItemList, mNoteItemList, true );

	if( mNodeItemList.isEmpty() && mGroupItemList.isEmpty() && mLinkItemList.isEmpty() && mNoteItemList.isEmpty() )
	{
		return;
	}

	CmdSetColour	*CMD = new CmdSetColour( pColor, mNodeItemList, mGroupItemList, mLinkItemList, mNoteItemList );

	if( CMD )
	{
		widget()->undoStack()->push( CMD );
	}

	clearTempLists();
}

void ContextView::updatePastePoint(QList<NodeItem *> NodeItemList, QList<NoteItem *> NoteItemList)
{
	if( !NodeItemList.isEmpty() )
	{
		m_PastePoint = NodeItemList.first()->pos();
	}
	else if( !NoteItemList.isEmpty() )
	{
		m_PastePoint = NoteItemList.first()->pos();
	}
	else
	{
		m_PastePoint = QPointF();
	}

	for( NodeItem *Node : NodeItemList )
	{
		m_PastePoint.rx() = qMin( Node->x(), m_PastePoint.x() );
		m_PastePoint.ry() = qMin( Node->y(), m_PastePoint.y() );
	}
}

QMap<QUuid, QUuid> ContextView::nodeGroups(QList<NodeItem *> pNodeList)
{
	QMap<QUuid, QUuid>		NodeGroups;

	for( NodeItem *NI : pNodeList )
	{
		if( !NI->groupId().isNull() )
		{
			NodeGroups.insert( NI->id(), NI->groupId() );
		}
	}

	return( NodeGroups );
}

void ContextView::cut()
{
	QSharedPointer<ContextPrivate> C = qSharedPointerCast<ContextPrivate>( mContext );

	if( !C )
	{
		return;
	}

	mPasteOffset = 0;

	QList<NodeItem *>  NodeItemList;
	QList<NodeItem *>  GroupItemList;
	QList<LinkItem *>  LinkItemList;
	QList<NoteItem *>  NoteItemList;

	QList<QSharedPointer<fugio::NodeInterface>>		NodeList;
	QList<QSharedPointer<NodeItem>>					GroupList;
	QMultiMap<QUuid,QUuid>							LinkList;
	QList<QSharedPointer<NoteItem>>					NoteList;

	QMap<QUuid,QUuid>								NodeGroups;

	if( !itemsForRemoval( NodeItemList, GroupItemList, LinkItemList, NoteItemList, NodeList, GroupList, LinkList, NoteList ) )
	{
		return;
	}

	QStringList		StuckNodes;

	for( NodeItem *NI : NodeItemList )
	{
		if( NI->hasPinsInGroup() )
		{
			StuckNodes << NI->name();
		}
	}

	if( !StuckNodes.isEmpty() )
	{
		QMessageBox::warning( this, tr( "Cannot cut" ), tr( "Can't cut the following nodes because they have pins being used by their parent group:\n\n%1" ).arg( StuckNodes.join( "\n" ) ), QMessageBox::Cancel, QMessageBox::NoButton );

		return;
	}

	updatePastePoint( NodeItemList, NoteItemList );

	// build up the list of nodes to save

	QList<QUuid>	SaveNodeList;

	SaveNodeList << nodeItemIds( NodeItemList );
	SaveNodeList << nodeItemIds( GroupItemList );

	mSaveOnlySelected = true;

	const QString		TempFile = QDir( QDir::tempPath() ).absoluteFilePath( "fugio-copy.tmp" );

	if( C->save( TempFile, &SaveNodeList ) )
	{
		QFile		FH( TempFile );

		if( FH.open( QFile::ReadOnly ) )
		{
			mPasteData = FH.readAll();

			FH.close();
		}
	}

	QFile::remove( TempFile );

	mSaveOnlySelected = false;

	CmdRemove		*Cmd = new CmdRemove( this, NodeList, GroupList, LinkList, NoteList, NodeGroups );

	if( Cmd )
	{
		widget()->undoStack()->push( Cmd );
	}
}

void ContextView::copy()
{
	QSharedPointer<ContextPrivate> C = qSharedPointerCast<ContextPrivate>( mContext );

	if( !C )
	{
		return;
	}

	resetPasteOffset();

	sortSelectedItems( mNodeItemList, mGroupItemList, mLinkItemList, mNoteItemList, true );

	if( mNodeItemList.isEmpty() && mGroupItemList.isEmpty() && mLinkItemList.isEmpty() && mNoteItemList.isEmpty() )
	{
		return;
	}

	QList<QUuid>	NodeList = nodeItemIds( mNodeItemList );

	updatePastePoint( mNodeItemList, mNoteItemList );

	mSaveOnlySelected = true;

	const QString		TempFile = QDir( QDir::tempPath() ).absoluteFilePath( "fugio-copy.tmp" );

	qDebug() << TempFile;

	if( C->save( TempFile, &NodeList ) )
	{
		QFile		FH( TempFile );

		if( FH.open( QFile::ReadOnly ) )
		{
			mPasteData = FH.readAll();

			FH.close();
		}
	}

	//QFile::remove( TempFile );

	mSaveOnlySelected = false;

	clearTempLists();
}

void ContextView::saveSelectedTo( const QString &pFileName )
{
	QSharedPointer<ContextPrivate> C = qSharedPointerCast<ContextPrivate>( mContext );

	if( !C )
	{
		return;
	}

	resetPasteOffset();

	sortSelectedItems( mNodeItemList, mGroupItemList, mLinkItemList, mNoteItemList, true );

	if( mNodeItemList.isEmpty() && mGroupItemList.isEmpty() && mLinkItemList.isEmpty() && mNoteItemList.isEmpty() )
	{
		return;
	}

	QList<QUuid>	NodeList = nodeItemIds( mNodeItemList );

	mSaveOnlySelected = true;

	if( C->save( pFileName, &NodeList ) )
	{
	}

	mSaveOnlySelected = false;

	clearTempLists();
}

void ContextView::paste()
{
	CmdContextViewPaste		*Cmd = new CmdContextViewPaste( this, mPasteData, m_PastePoint );

	widget()->undoStack()->push( Cmd );
}

void ContextView::dragEnterEvent( QDragEnterEvent *pEvent )
{
	if( pEvent->mimeData()->hasFormat( "application/x-qabstractitemmodeldatalist" ) )
	{
		pEvent->acceptProposedAction();
	}
	else if( pEvent->mimeData()->hasUrls() )
	{
		pEvent->acceptProposedAction();
	}
	else
	{
		QGraphicsView::dragEnterEvent( pEvent );
	}
}

void ContextView::dropEvent( QDropEvent *pEvent )
{
	if( pEvent->mimeData()->hasFormat( "application/x-qabstractitemmodeldatalist" ) )
	{
		QByteArray encoded = pEvent->mimeData()->data( "application/x-qabstractitemmodeldatalist" );

		QDataStream stream(&encoded, QIODevice::ReadOnly);

		while (!stream.atEnd())
		{
			int row, col;
			QMap<int,  QVariant> roleDataMap;
			stream >> row >> col >> roleDataMap;

			if( roleDataMap.contains( Qt::DisplayRole ) )
			{
				QString			NodeName = roleDataMap.value( Qt::DisplayRole ).toString();
				QUuid			NodeUuid = roleDataMap.value( Qt::UserRole ).toUuid();

				if( NodeUuid.isNull() )
				{
					qDebug() << "Node UUID for" << NodeName << "is NULL";
				}
				else
				{
					CmdNodeAdd		*Cmd = new CmdNodeAdd( widget(), NodeName, NodeUuid );

					setNodePositionFlag();

					mNodePosition = mapToScene( pEvent->pos() );

					resetPasteOffset();

					widget()->undoStack()->push( Cmd );
				}
			}
		}

		pEvent->acceptProposedAction();

		setFocus();
	}
	else if( pEvent->mimeData()->hasUrls() )
	{
		for( auto Url : pEvent->mimeData()->urls() )
		{
			QByteArray		FileData;

			if( Url.isLocalFile() )
			{
				QFile		FH( Url.toLocalFile() );

				if( FH.open( QFile::ReadOnly ) )
				{
					FileData = FH.readAll();

					FH.close();
				}

			}

			if( !FileData.isEmpty() )
			{
				CmdContextViewPaste		*Cmd = new CmdContextViewPaste( this, FileData, pEvent->pos() );

				widget()->undoStack()->push( Cmd );
			}
		}

		pEvent->acceptProposedAction();

		setFocus();
	}
	else
	{
		QGraphicsView::dropEvent( pEvent );
	}
}

void ContextView::dragMoveEvent( QDragMoveEvent *pEvent )
{
	if( pEvent->mimeData()->hasFormat( "application/x-qabstractitemmodeldatalist" ) )
	{
		pEvent->acceptProposedAction();
	}
	else if( pEvent->mimeData()->hasUrls() )
	{
		pEvent->acceptProposedAction();
	}
	else
	{
		QGraphicsView::dragMoveEvent( pEvent );
	}
}

void ContextView::focusInEvent( QFocusEvent *pEvent )
{
	mContext->global()->setEditTarget( this );

	QGraphicsView::focusInEvent( pEvent );
}

void ContextView::focusOutEvent( QFocusEvent *pEvent )
{
	mContext->global()->setEditTarget( nullptr );

	QGraphicsView::focusOutEvent( pEvent );
}

void ContextView::showWizard()
{
	if( !mShownWizard && isVisible() )
	{
		QSettings		Settings;

		mShownWizard = true;

		int		CurVer = Settings.value( "node-editor-wizard", 0 ).toInt();

		if( CurVer != NodeEditorWizard::version() )
		{
			NodeEditorWizard	Wizard( this );

			if( Wizard.exec() == QWizard::Accepted )
			{
				Settings.setValue( "node-editor-wizard", NodeEditorWizard::version() );
			}
		}
	}
}

void ContextView::showEvent( QShowEvent *pEvent )
{
	if( !mShownWizard && isVisible() )
	{
		QTimer::singleShot( 500, this, SLOT(showWizard()) );
	}

	QGraphicsView::showEvent( pEvent );
}

void ContextView::nodeMoved( const QUuid &pUuid, const QPointF &P1, const QPointF &P2 )
{
	mNodeMoveData.insert( pUuid, MoveData( P1, P2 ) );
}

void ContextView::noteMoved( NoteItem *pNoteItem, const QPointF &P1, const QPointF &P2 )
{
	for( QSharedPointer<NoteItem> NI : mNoteList )
	{
		if( NI.data() != pNoteItem )
		{
			continue;
		}

		mNoteMoveData.insert( NI, MoveData( P1, P2 ) );

		break;
	}
}

void ContextView::nodeMoveStarted()
{
	mNodeMoveUndoId++;

	mNodeMoveData.clear();
	mNoteMoveData.clear();

	for( QGraphicsItem *Item : scene()->selectedItems() )
	{
		if( NodeItem *NI = qgraphicsitem_cast<NodeItem *>( Item ) )
		{
			NI->moveStarted();

			continue;
		}

		if( NoteItem *NI = qgraphicsitem_cast<NoteItem *>( Item ) )
		{
			NI->moveStarted();

			continue;
		}
	}
}

void ContextView::nodeMoveFinished()
{
	for( QUuid NodeId : mNodeMoveData.keys() )
	{
		MoveData		MD = mNodeMoveData.value( NodeId );

		if( MD.first == MD.second )
		{
			mNodeMoveData.remove( NodeId );
		}
	}

	for( QSharedPointer<NoteItem> NI : mNoteMoveData.keys() )
	{
		MoveData		MD = mNoteMoveData.value( NI );

		if( MD.first == MD.second )
		{
			mNoteMoveData.remove( NI );
		}
	}

	if( mNodeMoveData.isEmpty() && mNoteMoveData.isEmpty() )
	{
		return;
	}

	CmdMove		*Cmd = new CmdMove( this, mNodeMoveData, mNoteMoveData );

	if( Cmd != 0 )
	{
		widget()->undoStack()->push( Cmd );
	}

	mNodeMoveData.clear();
	mNoteMoveData.clear();
}

void ContextView::noteAdd( QSharedPointer<NoteItem> pNoteItem )
{
	scene()->addItem( pNoteItem.data() );

	mNoteList.append( pNoteItem );
}

void ContextView::noteRemove( QSharedPointer<NoteItem> pNoteItem )
{
	scene()->removeItem( pNoteItem.data() );

	mNoteList.removeAll( pNoteItem );
}

bool ContextView::isGroupEmpty( const QUuid &pGroupId ) const
{
	for( QGraphicsItem *Item : scene()->items() )
	{
		if( NodeItem *NI = qgraphicsitem_cast<NodeItem *>( Item ) )
		{
			if( NI->groupId() == pGroupId )
			{
				return( false );
			}

			continue;
		}

		if( NoteItem *NI = qgraphicsitem_cast<NoteItem *>( Item ) )
		{
			if( NI->groupId() == pGroupId )
			{
				return( false );
			}

			continue;
		}
	}

	return( true );
}

void ContextView::clearTempLists()
{
	mNodeItemList.clear();
	mGroupItemList.clear();
	mLinkItemList.clear();
	mNoteItemList.clear();
}

void ContextView::moveNodeRelative( NodeItem *pNode, const QList<NodeItem *> &pNodeList, const QList<NoteItem *> &pNoteList, ContextView::NodeMoveRelation pRelation )
{
	QPointF		GrpPos = pNode->pos();
	QRectF		GrpRct;

	for( NodeItem *N : pNodeList )
	{
		GrpRct = GrpRct.united( N->boundingRect().translated( N->pos() ) );
	}

	for( NoteItem *N : pNoteList )
	{
		GrpRct = GrpRct.united( N->boundingRect().translated( N->pos() ) );
	}

	switch( pRelation )
	{
		case TO_LEFT:
			GrpPos = GrpRct.topLeft() - QPointF( pNode->boundingRect().size().width() + 10, 0 );
			break;

		case TO_RIGHT:
			GrpPos = GrpRct.topRight() + QPointF( 10, 0 );
			break;

		case TO_CENTER:
			GrpPos = GrpRct.center() - QPointF( pNode->boundingRect().size().width() / 2, pNode->boundingRect().size().height() / 2 );
			break;
	}

	GrpPos = NodeItem::computeTopLeftGridPoint( GrpPos );

	pNode->setNodePosSilent( GrpPos );
}

void ContextView::processGroupLinks( QSharedPointer<NodeItem> NI)
{
	const QUuid		NewGroupId = NI->id();

	for( QGraphicsItem *Item : scene()->items() )
	{
		LinkItem		*LI = qgraphicsitem_cast<LinkItem *>( Item );

		if( !LI )
		{
			continue;
		}

		PinItem			*SrcPin = LI->srcPin();
		PinItem			*DstPin = LI->dstPin();
		PinItem			*NewPin = nullptr;

		NodeItem		*SrcNod = qgraphicsitem_cast<NodeItem *>( SrcPin->parentItem() );
		NodeItem		*DstNod = qgraphicsitem_cast<NodeItem *>( DstPin->parentItem() );

		if( SrcNod->groupId() == NewGroupId && DstNod->groupId() != NewGroupId )
		{
			QSharedPointer<NodeItem>	GroupItem = findNodeItem( DstNod->groupId() );

			if( !GroupItem || GroupItem->groupId() != NewGroupId )
			{
				if( ( NewPin = NI->findPinOutput( SrcPin->pin() ) ) == nullptr )
				{
					if( ( NewPin = NI->pinOutputAdd( SrcPin->pin() ) ) )
					{
						NewPin->setName( SrcPin->name() );

						NewPin->setColour( SrcPin->colour() );

						connect( NewPin, SIGNAL(colourUpdated(QColor)), SrcPin, SLOT(setColour(QColor)) );
						connect( SrcPin, SIGNAL(colourUpdated(QColor)), NewPin, SLOT(setColour(QColor)) );
					}
				}

				if( NewPin )
				{
					if( LinkItem *Link = new LinkItem() )
					{
						scene()->addItem( Link );

						Link->setSrcPin( NewPin );
						Link->setDstPin( DstPin );

						NewPin->linkAdd( Link );
						DstPin->linkAdd( Link );
					}
				}
			}
		}

		if( SrcNod->groupId() != NewGroupId && DstNod->groupId() == NewGroupId )
		{
			QSharedPointer<NodeItem>	GroupItem = findNodeItem( SrcNod->groupId() );

			if( !GroupItem || GroupItem->groupId() != NewGroupId )
			{
				if( ( NewPin = NI->findPinInput( DstPin->pin() ) ) == nullptr )
				{
					if( ( NewPin = NI->pinInputAdd( DstPin->pin() ) ) )
					{
						NewPin->setName( DstPin->name() );
					}
				}

				if( NewPin )
				{
					if( LinkItem *Link = new LinkItem() )
					{
						scene()->addItem( Link );

						Link->setSrcPin( SrcPin );
						Link->setDstPin( NewPin );

						SrcPin->linkAdd( Link );
						NewPin->linkAdd( Link );
					}
				}
			}
		}
	}

	NI->layoutPins();
}

QUuid ContextView::group( const QString &pGroupName, QList<NodeItem *> &pNodeList, QList<NodeItem *> &pGroupList, QList<NoteItem *> &pNoteList, const QUuid &pGroupId )
{
#if defined( USE_CONTEXT_MODEL )
	const QUuid		NewGroupId = mContextModel.createGroup( pGroupId, pGroupName );
#else
	const QUuid		NewGroupId = ( pGroupId.isNull() ? QUuid::createUuid() : pGroupId );
#endif

	if( !mGroupIds.contains( NewGroupId ) )
	{
		mGroupIds << NewGroupId;
	}

	for( NodeItem *NI : pNodeList )
	{
		NI->setGroupId( NewGroupId );
	}

	for( NodeItem *NI : pGroupList )
	{
		NI->setGroupId( NewGroupId );
	}

	for( NoteItem *NI : pNoteList )
	{
		NI->setGroupId( NewGroupId );
	}

	if( !mNodeList.contains( NewGroupId ) )
	{
		nodeAdded( NewGroupId, NewGroupId );
	}

#if defined( USE_CONTEXT_MODEL )
	mContextModel.moveToGroup( NewGroupId, nodeItemIds( pNodeList ), nodeItemIds( pGroupList ), noteItemIds( pNoteList ) );
#endif

	QSharedPointer<NodeItem>	NI = mNodeList[ NewGroupId ];

	NI->setName( pGroupName );

	NI->setIsGroup( true );

	// Calculate the position of the group as the center of the nodes/notes

	moveNodeRelative( NI.data(), pNodeList, pNoteList, TO_CENTER );

	processGroupLinks( NI );

	updateItemVisibility();

	NI->labelDrag( QPointF() );

	return( NewGroupId );
}

void ContextView::ungroup( QList<NodeItem *> &pNodeList, QList<NodeItem *> &pGroupList, QList<NoteItem *> &pNoteList, const QUuid &pGroupId )
{
	// Find the NodeItem for the supplied GroupId

	QSharedPointer<NodeItem>	GI = mNodeList[ pGroupId ];

	if( !GI )
	{
		qWarning() << "Couldn't find group" << pGroupId;
	}

	// Search for any links that link from the ungrouping nodes to outside nodes
	// and delete them

	for( QGraphicsItem *Item : scene()->items() )
	{
		LinkItem		*LI = qgraphicsitem_cast<LinkItem *>( Item );

		if( !LI )
		{
			continue;
		}

		PinItem			*SrcPin = LI->srcPin();
		PinItem			*DstPin = LI->dstPin();

		NodeItem		*SrcNod = qgraphicsitem_cast<NodeItem *>( SrcPin->parentItem() );
		NodeItem		*DstNod = qgraphicsitem_cast<NodeItem *>( DstPin->parentItem() );

		bool			 SrcIsGrp = ( SrcNod->id() == pGroupId );
		bool			 DstIsGrp = ( DstNod->id() == pGroupId );

		if( !SrcIsGrp && !DstIsGrp )
		{
			continue;
		}

		bool			 LnkRem = false;

		if( SrcIsGrp )
		{
			for( NodeItem *NI : pNodeList )
			{
				if( NI->findPinOutput( SrcPin->uuid() ) )
				{
					LnkRem = true;

					break;
				}
			}
		}
		else
		{
			for( NodeItem *NI : pNodeList )
			{
				if( NI->findPinInput( DstPin->uuid() ) )
				{
					LnkRem = true;

					break;
				}
			}
		}

		if( !LnkRem )
		{
			continue;
		}

		scene()->removeItem( LI );

		LI->setSrcPin( nullptr );
		LI->setDstPin( nullptr );

		SrcPin->linkRem( LI );
		DstPin->linkRem( LI );

		delete LI;

		if( GI )
		{
			if( SrcIsGrp )
			{
				GI->pinOutputRemove( SrcPin->uuid() );
			}
			else
			{
				GI->pinInputRemove( DstPin->uuid() );
			}
		}
	}

	for( NodeItem *NI : pNodeList )
	{
		NI->setGroupId( groupId() );
	}

	for( NodeItem *NI : pGroupList )
	{
		NI->setGroupId( groupId() );
	}

	for( NoteItem *NI : pNoteList )
	{
		NI->setGroupId( groupId() );
	}

	if( isGroupEmpty( pGroupId ) )
	{
		mGroupIds.removeAll( pGroupId );

		nodeRemoved( pGroupId );
	}
	else if( GI )
	{
		GI->layoutPins();
	}

	updateItemVisibility();
}

void ContextView::ungroup( NodeItem *GI )
{
	for( QGraphicsItem *Item : scene()->items() )
	{
		if( Item == GI )
		{
			continue;
		}

		NodeItem		*NI = qgraphicsitem_cast<NodeItem *>( Item );

		if( NI )
		{
			if( NI->groupId() == GI->id() )
			{
				NI->setGroupId( GI->groupId() );
			}

			continue;
		}

		LinkItem		*LI = qgraphicsitem_cast<LinkItem *>( Item );

		if( LI )
		{
			PinItem			*SrcPin = LI->srcPin();
			PinItem			*DstPin = LI->dstPin();

			NodeItem		*SrcNod = qgraphicsitem_cast<NodeItem *>( SrcPin->parentItem() );
			NodeItem		*DstNod = qgraphicsitem_cast<NodeItem *>( DstPin->parentItem() );

			if( SrcNod->id() == GI->id() || DstNod->id() == GI->id() )
			{
				delete LI;
			}

			continue;
		}

		NoteItem		*TI = qgraphicsitem_cast<NoteItem *>( Item );

		if( TI )
		{
			if( TI->groupId() == GI->id() )
			{
				TI->setGroupId( GI->groupId() );
			}

			continue;
		}
	}

	scene()->removeItem( GI );

	mGroupIds.removeAll( GI->id() );

	mNodeList.remove( GI->id() );

	updateItemVisibility();
}

void ContextView::groupSelected()
{
	sortSelectedItems( mNodeItemList, mGroupItemList, mLinkItemList, mNoteItemList, true );

	if( mNodeItemList.isEmpty() && mGroupItemList.isEmpty() && mLinkItemList.isEmpty() && mNoteItemList.isEmpty() )
	{
		return;
	}

	const static QString	DefaultGroupName = tr( "New Group" );

	QString		GroupName = QInputDialog::getText( nullptr, tr( "Group Name" ), tr( "Enter a name for this group" ), QLineEdit::Normal, DefaultGroupName );

	if( GroupName.isEmpty() )
	{
		GroupName = DefaultGroupName;
	}

	CmdGroup	*CMD = new CmdGroup( this, GroupName, mNodeItemList, mGroupItemList, mNoteItemList );

	if( CMD )
	{
		widget()->undoStack()->push( CMD );
	}

	clearTempLists();
}

void ContextView::ungroupSelected()
{
	sortSelectedItems( mNodeItemList, mGroupItemList, mLinkItemList, mNoteItemList, true );

	if( mNodeItemList.isEmpty() && mGroupItemList.isEmpty() && mLinkItemList.isEmpty() && mNoteItemList.isEmpty() )
	{
		return;
	}

	// TODO: Add Undo

	for( NodeItem *NI : mNodeItemList )
	{
		if( !mContext->findNode( NI->id() ) )
		{
			ungroup( NI );
		}
	}

//	CmdUngroup	*CMD = new CmdUngroup( this, NodeItemList, NoteItemList );

//	if( CMD )
//	{
//		widget()->undoStack()->push( CMD );
//	}

	clearTempLists();
}

void ContextView::groupParent()
{
	if( mGroupStack.isEmpty() )
	{
		return;
	}

	CmdGroupPop	*CMD = new CmdGroupPop( this );

	if( CMD )
	{
		widget()->undoStack()->push( CMD );
	}
}

void ContextView::pushGroup( const QUuid &pGroupId )
{
	Q_ASSERT( mGroupIds.contains( pGroupId ) );

	mGroupStack << pGroupId;

	setGroupId( pGroupId );

#if defined( USE_CONTEXT_MODEL )
	mContextModel.setCurrentGroup( pGroupId );
#endif
}

void ContextView::popGroup()
{
	if( !mGroupStack.isEmpty() )
	{
		mGroupStack.pop_back();
	}

	QUuid		GroupId;

	if( !mGroupStack.isEmpty() )
	{
		GroupId = mGroupStack.back();
	}

	setGroupId( GroupId );

#if defined( USE_CONTEXT_MODEL )
	mContextModel.setCurrentGroup( GroupId );
#endif
}

bool ContextView::event( QEvent *pEvent )
{
	if( pEvent->type() == QEvent::Gesture )
	{
		return gestureEvent( static_cast<QGestureEvent*>( pEvent ) );
	}

	return( QGraphicsView::event( pEvent ) );
}

bool ContextView::gestureEvent(QGestureEvent *pEvent)
{
	//qDebug() << "gestureEvent():" << pEvent;

	if( QGesture *pan = pEvent->gesture( Qt::PanGesture ) )
	{
		panTriggered( static_cast<QPanGesture *>( pan ) );
	}

	if( QGesture *pinch = pEvent->gesture( Qt::PinchGesture ) )
	{
		pinchTriggered( static_cast<QPinchGesture *>( pinch ) );
	}

	return( true );
}

void ContextView::panTriggered( QPanGesture *pGesture )
{
	qDebug() << pGesture->delta();
}

void ContextView::pinchTriggered( QPinchGesture *pGesture )
{
	zoom( pGesture->scaleFactor(), pGesture->centerPoint() );
}

void ContextView::wheelEvent( QWheelEvent *pEvent )
{
	QPoint numPixels = pEvent->pixelDelta();
	QPoint numDegrees = pEvent->angleDelta() / 8;

	if( !numPixels.isNull() )
	{
		horizontalScrollBar()->setValue( horizontalScrollBar()->value() - numPixels.x() );

		verticalScrollBar()->setValue( verticalScrollBar()->value() - numPixels.y() );
	}
	else if( !numDegrees.isNull() )
	{
		if( pEvent->modifiers().testFlag( Qt::ControlModifier ) )
		{
			int numSteps = pEvent->delta() / 15 / 8;

			qreal sc = pow( 1.25, numSteps );

			zoom( sc, mapToScene( pEvent->pos() ) );
		}
		else
		{
			horizontalScrollBar()->setValue( horizontalScrollBar()->value() - numDegrees.x() );

			verticalScrollBar()->setValue( verticalScrollBar()->value() - numDegrees.y() );
		}
	}

	pEvent->accept();
}

void ContextView::zoom( qreal factor, QPointF centerPoint )
{
	//factor = qBound( 0.9, factor, 1.1 );

	QTransform	Transform;

	Transform.scale( factor, factor );

	Transform.translate( centerPoint.x(), centerPoint.y() );

	setTransform( Transform );
}

