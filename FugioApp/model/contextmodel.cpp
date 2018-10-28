#include "contextmodel.h"

#include <fugio/context_signals.h>

#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/node_signals.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_signals.h>

#include "basemodel.h"
#include "pinmodel.h"
#include "notemodel.h"
#include "baselistmodel.h"

#include "app.h"
#include "mainwindow.h"

#include <QTreeView>
#include <QDockWidget>

ContextModel::ContextModel( QObject *pParent )
	: QAbstractItemModel( pParent )
{
	mRootItem = new BaseListModel();
}

ContextModel::~ContextModel()
{
	clearContext();
}

void ContextModel::setContext( fugio::ContextInterface *pContext )
{
	clearContext();

	mContext = pContext;

	fugio::ContextSignals		*CS = mContext->qobject();

	connect( CS, SIGNAL(clearContext()), this, SLOT(clearContext()) );

	connect( CS, SIGNAL(nodeAdded(QUuid)), this, SLOT(nodeAdded(QUuid)) );
	connect( CS, SIGNAL(nodeRemoved(QUuid)), this, SLOT(nodeRemoved(QUuid)) );
	connect( CS, SIGNAL(nodeRenamed(QUuid,QUuid)), this, SLOT(nodeRenamed(QUuid,QUuid)) );

	connect( CS, SIGNAL(pinAdded(QUuid,QUuid)), this, SLOT(pinAdded(QUuid,QUuid)) );
	connect( CS, SIGNAL(pinRemoved(QUuid,QUuid)), this, SLOT(pinRemoved(QUuid,QUuid)) );
	connect( CS, SIGNAL(pinRenamed(QUuid,QUuid,QUuid)), this, SLOT(pinRenamed(QUuid,QUuid,QUuid)) );

	connect( CS, SIGNAL(loadStart(QSettings&,bool)), this, SLOT(loadStarted(QSettings&,bool)) );
	connect( CS, SIGNAL(loadEnd(QSettings&,bool)), this, SLOT(loadEnded(QSettings&,bool)) );

	//	if( QSharedPointer<ContextPrivate> C = qSharedPointerCast<ContextPrivate>( mContext ) )
	//	{
	//		connect( C.data(), SIGNAL(loading(QSettings&,bool)), this, SLOT(loadContext(QSettings&,bool)) );

	//		connect( C.data(), SIGNAL(saving(QSettings&)), this, SLOT(saveContext(QSettings&)) );

	//		connect( C.data(), SIGNAL(nodeUpdated(QUuid)), this, SLOT(nodeChanged(QUuid)) );
	//		connect( C.data(), SIGNAL(nodeActivated(QUuid)), this, SLOT(nodeActivated(QUuid)) );

	//		connect( C.data(), SIGNAL(linkAdded(QUuid,QUuid)), this, SLOT(linkAdded(QUuid,QUuid)) );
	//		connect( C.data(), SIGNAL(linkRemoved(QUuid,QUuid)), this, SLOT(linkRemoved(QUuid,QUuid)) );
	//	}


	MainWindow		*MW = gApp->mainWindow();

	QDockWidget		*DW = new QDockWidget( "Context Model", MW );

	DW->setObjectName( "ContextModel" );

	QTreeView		*TV = new QTreeView( DW );

	TV->setModel( this );

	DW->setWidget( TV );

	MW->addDockWidget( Qt::RightDockWidgetArea, DW );
}

void ContextModel::setCurrentGroup( const QUuid &pGroupId )
{
	mCurrentGroup = pGroupId;
}

//-----------------------------------------------------------------------------

QUuid ContextModel::createGroup( const QUuid &pGroupId, QString pGroupName )
{
	BaseListModel		*Parent = ( mCurrentGroup.isNull() ? mRootItem : mNodeMap.value( mCurrentGroup )->children() );

	if( !Parent )
	{
		return( QUuid() );
	}

	QUuid				 GroupId = pGroupId.isNull() ? QUuid::createUuid() : pGroupId;

	NodeModel			*Group = new NodeModel( GroupId, Parent );

	if( !Group )
	{
		return( QUuid() );
	}

	Group->setName( pGroupName );

	int					 GroupIdx = Parent->rowCount();

	beginInsertRows( createIndex( Parent->row(), 0, Parent ), GroupIdx, GroupIdx );

	Parent->appendChild( Group );

	mNodeMap.insert( GroupId, Group );

	endInsertRows();

	emit layoutChanged();

	emit groupAdded( GroupId );

	return( GroupId );
}

void ContextModel::removeGroup( const QUuid &pGroupId )
{
	NodeModel			*Group = mNodeMap.value( pGroupId );

	if( Group )
	{
		BaseListModel	*Parent = Group->parentList();
		int				 GroupIndex = Group->row();

		beginRemoveRows( createIndex( Parent->row(), 0, Parent ), GroupIndex, GroupIndex );

		Parent->removeChildAt( GroupIndex );

		mNodeMap.remove( pGroupId );

		endRemoveRows();

		emit layoutChanged();

		delete Group;
	}
}

void ContextModel::removeNote( const QUuid &pNoteId )
{
	NoteModel			*Note = mNoteMap.value( pNoteId );

	if( Note )
	{
		BaseListModel	*Parent = Note->parentList();
		int				 NoteIndex = Note->row();

		beginRemoveRows( createIndex( Parent->row(), 0, Parent ), NoteIndex, NoteIndex );

		Parent->removeChildAt( NoteIndex );

		mNoteMap.remove( pNoteId );

		endRemoveRows();

		emit layoutChanged();

		delete Note;
	}
}

void ContextModel::moveToGroup( const QUuid &pGroupId, const QList<QUuid> &pNodeList, const QList<QUuid> &pGroupList, const QList<QUuid> &pNoteList)
{
	BaseListModel		*GroupDest = mNodeMap.value( pGroupId )->children();

	if( !GroupDest )
	{
		return;
	}

	for( const QUuid &Id : pNodeList )
	{
		NodeModel		*Node = mNodeMap.value( Id );

		if( !Node )
		{
			continue;
		}

		BaseListModel	*GroupSource = Node->parentList();

		if( !GroupSource || GroupSource == GroupDest )
		{
			continue;
		}

		const int		NodeRow = Node->row();

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), NodeRow, NodeRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount() );

		GroupSource->removeChildAt( NodeRow );

		GroupDest->appendChild( Node );

		Node->setParent( GroupDest );

		endMoveRows();
	}

	for( const QUuid &Id : pGroupList )
	{
		NodeModel		*Group = mNodeMap.value( Id );

		if( !Group )
		{
			continue;
		}

		BaseListModel	*GroupSource = Group->parentList();

		if( !GroupSource || GroupSource == GroupDest )
		{
			continue;
		}

		const int		GroupRow = Group->row();

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), GroupRow, GroupRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount() );

		GroupSource->removeChildAt( GroupRow );

		GroupDest->appendChild( Group );

		Group->setParent( GroupDest );

		endMoveRows();
	}

	for( const QUuid &Id : pNoteList )
	{
		NoteModel		*Note = mNoteMap.value( Id );

		if( !Note )
		{
			continue;
		}

		BaseListModel	*GroupSource = Note->parentList();

		if( !GroupSource || GroupSource == GroupDest )
		{
			continue;
		}

		const int		NoteRow = Note->row();

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), NoteRow, NoteRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount() );

		GroupSource->removeChildAt( NoteRow );

		GroupDest->appendChild( Note );

		Note->setParent( GroupDest );

		endMoveRows();
	}

	emit layoutChanged();
}

QUuid ContextModel::createNote( QUuid pUuid )
{
	BaseListModel		*Parent = ( mCurrentGroup.isNull() ? mRootItem : mNodeMap.value( mCurrentGroup )->children() );

	if( !Parent )
	{
		return( QUuid() );
	}

	NoteModel			*Note = new NoteModel( Parent );

	if( !Note )
	{
		return( QUuid() );
	}

	QUuid				 NoteId = ( pUuid.isNull() ? QUuid::createUuid() : pUuid );

	int					 NoteIdx = Parent->rowCount();

	beginInsertRows( createIndex( Parent->row(), 0, Parent ), NoteIdx, NoteIdx );

	Parent->appendChild( Note );

	mNoteMap.insert( NoteId, Note );

	endInsertRows();

	emit layoutChanged();

	emit noteAdded( NoteId );

	return( NoteId );
}

QModelIndex ContextModel::nodeIndex( const QUuid &pId ) const
{
	NodeModel		*Node = mNodeMap.value( pId );

	return( Node ? createIndex( Node->row(), 0, Node ) : QModelIndex() );
}

QModelIndex ContextModel::noteIndex( const QUuid &pId ) const
{
	NoteModel		*Note = mNoteMap.value( pId );

	return( Note ? createIndex( Note->row(), 0, Note ) : QModelIndex() );
}

QModelIndex ContextModel::pinIndex( const QUuid &pId ) const
{
	PinModel		*Pin = mPinMap.value( pId );

	return( Pin ? createIndex( Pin->row(), Pin->direction() == PIN_INPUT ? 0 : 1, Pin ) : QModelIndex() );
}

void ContextModel::loadStarted(QSettings &pSettings, bool pPartial)
{
	Q_UNUSED( pSettings )
	Q_UNUSED( pPartial )

	emit layoutAboutToBeChanged();
}

void ContextModel::loadEnded( QSettings &pSettings, bool pPartial )
{
	Q_UNUSED( pSettings )
	Q_UNUSED( pPartial )

	changePersistentIndex( createIndex( 0, 0, mRootItem ), createIndex( mRootItem->rowCount(), 0, mRootItem ) );

	emit layoutChanged();
}

//-----------------------------------------------------------------------------
// fugio::ContextSignals

void ContextModel::clearContext()
{
	beginResetModel();

	mNodeMap.clear();

	mNoteMap.clear();

	mPinMap.clear();

	if( mRootItem )
	{
		delete mRootItem;
	}

	mRootItem = new BaseListModel();

	endResetModel();
}

void ContextModel::nodeAdded( QUuid pNodeId )
{
	BaseListModel	*Parent = ( mCurrentGroup.isNull() ? mRootItem : mNodeMap.value( mCurrentGroup )->children() );

	qDebug() << "nodeAdded" << pNodeId;

	const int		 ChildCount = Parent->rowCount();

	NodeModel		*Node = new NodeModel( pNodeId, Parent );

	if( Node )
	{
		QSharedPointer<fugio::NodeInterface>	NI = mContext->findNode( pNodeId );

		if( NI )
		{
			fugio::NodeSignals		*NS = NI->qobject();

			connect( NS, SIGNAL(nameChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(nodeNameChanged(QSharedPointer<fugio::NodeInterface>)) );

			Node->setName( NI->name() );
		}

		beginInsertRows( createIndex( Parent->row(), 0, Parent ), ChildCount, ChildCount );

		Parent->appendChild( Node );

		mNodeMap.insert( pNodeId, Node );

		endInsertRows();
	}

	emit layoutChanged();
}

void ContextModel::nodeRemoved( QUuid pNodeId )
{
	NodeModel		*Node = mNodeMap.value( pNodeId );

	if( Node )
	{
		BaseListModel		*Parent = Node->parentList();

		if( Parent )
		{
			const int		 Index = Parent->childRow( Node );

			beginRemoveRows( createIndex( Parent->row(), 0, Parent ), Index, Index );

			Parent->removeChildAt( Index );

			endRemoveRows();
		}

		mNodeMap.remove( pNodeId );

		delete Node;
	}

	QSharedPointer<fugio::NodeInterface>	NI = mContext->findNode( pNodeId );

	if( NI )
	{
		fugio::NodeSignals		*NS = NI->qobject();

		disconnect( NS, SIGNAL(nameChanged(QSharedPointer<fugio::NodeInterface>)), this, SLOT(nodeNameChanged(QSharedPointer<fugio::NodeInterface>)) );
	}
}

void ContextModel::nodeRenamed( QUuid pOrigId, QUuid pNodeId )
{
	NodeModel		*Node = mNodeMap.value( pOrigId );

	if( Node )
	{
		mNodeMap.remove( pOrigId );

		Node->setNodeId( pNodeId );

		mNodeMap.insert( pNodeId, Node );
	}
}

void ContextModel::pinAdded( QUuid pNodeId, QUuid pPinId )
{
	NodeModel		*Node = mNodeMap.value( pNodeId );

	if( Node )
	{
		QSharedPointer<fugio::PinInterface>	PI = mContext->findPin( pPinId );

		if( PI )
		{
			fugio::PinSignals		*PS = PI->qobject();

			connect( PS, SIGNAL(nameChanged(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinNameChanged(QSharedPointer<fugio::PinInterface>)) );

			PinListModel	*PinList = ( PI->direction() == PIN_INPUT ? Node->inputs() : Node->outputs() );

			beginInsertRows( createIndex( PinList->row(), 0, PinList ), PinList->rowCount(), PinList->rowCount() );

			PinModel		*Pin = PinList->appendPin( pPinId, PI->name() );

			mPinMap.insert( pPinId, Pin );

			endInsertRows();
		}
	}
}

void ContextModel::pinRemoved( QUuid pNodeId, QUuid pPinId )
{
	NodeModel		*Node = mNodeMap.value( pNodeId );
	PinModel		*Pin  = mPinMap.value( pPinId );

	if( Node && Pin )
	{
		const int		 PinRow = Pin->row();

		PinListModel	*PinList = ( Pin->direction() == PIN_INPUT ? Node->inputs() : Node->outputs() );

		beginRemoveRows( createIndex( PinList->row(), 0, PinList ), PinRow, PinRow );

		PinList->remPin( Pin );

		endRemoveRows();

		mPinMap.remove( pPinId );

		delete Pin;
	}
}

void ContextModel::pinRenamed( QUuid pNodeId, QUuid pOldId, QUuid pNewId )
{
	Q_UNUSED( pNodeId )

	PinModel		*Pin = mPinMap.value( pOldId );

	if( Pin )
	{
		mPinMap.remove( pOldId );

		Pin->setId( pNewId );

		mPinMap.insert( pNewId, Pin );
	}
}

void ContextModel::nodeNameChanged( QSharedPointer<fugio::NodeInterface> pNode )
{
	NodeModel		*Node = mNodeMap.value( pNode->uuid() );

	if( Node )
	{
		Node->setName( pNode->name() );

		emit layoutChanged();
	}
}

void ContextModel::pinNameChanged( QSharedPointer<fugio::PinInterface> pPin )
{
	PinModel		*Pin = mPinMap.value( pPin->globalId() );

	if( Pin )
	{
		Pin->setName( pPin->name() );

		emit layoutChanged();
	}
}

//-----------------------------------------------------------------------------
// QAbstractItemModel

QModelIndex ContextModel::index( int row, int column, const QModelIndex &parent ) const
{
	if( !hasIndex( row, column, parent ) )
	{
		return QModelIndex();
	}

	BaseModel		*Parent;

	if( parent.isValid() )
	{
		Parent = static_cast<BaseModel *>( parent.internalPointer() );
	}
	else
	{
		Parent = mRootItem;
	}

	NodeModel		*NodeParent = dynamic_cast<NodeModel *>( Parent );

	if( NodeParent )
	{
		BaseModel			*Child = nullptr;

		if( row == 0 ) Child = NodeParent->inputs();
		if( row == 1 ) Child = NodeParent->outputs();
		if( row == 2 ) Child = NodeParent->children();

		if( Child )
		{
			return( createIndex( row, column, Child ) );
		}

		return( QModelIndex() );
	}

	PinListModel	*PinListParent = dynamic_cast<PinListModel *>( Parent );

	if( PinListParent )
	{
		PinModel		*Child = PinListParent->childAt( row );

		if( Child )
		{
			return( createIndex( row, column, Child ) );
		}

		return( QModelIndex() );
	}

	BaseListModel	*BaseListParent = dynamic_cast<BaseListModel *>( Parent );

	if( BaseListParent )
	{
		BaseModel		*Child = BaseListParent->childAt( row );

		if( Child )
		{
			return( createIndex( row, column, Child ) );
		}

		return( QModelIndex() );
	}

	return( QModelIndex() );
}

QModelIndex ContextModel::parent( const QModelIndex &pChildIndex ) const
{
	if( !pChildIndex.isValid() )
	{
		return( QModelIndex() );
	}

	BaseModel		*Child = static_cast<BaseModel *>( pChildIndex.internalPointer() );

	if( Child == mRootItem )
	{
		return( QModelIndex() );
	}

	BaseModel		*Parent = Child->parent();

	if( Parent == mRootItem )
	{
		return( QModelIndex() );
	}

	return( createIndex( Parent->row(), 0, Parent ) );
}

int ContextModel::rowCount( const QModelIndex &pParent ) const
{
	if( pParent.column() > 0 )
	{
		return( 0 );
	}

	BaseModel		*Parent;

	if( pParent.isValid() )
	{
		Parent = static_cast<BaseModel *>( pParent.internalPointer() );
	}
	else
	{
		Parent = mRootItem;
	}

	return( Parent->rowCount() );
}

int ContextModel::columnCount( const QModelIndex &pParent ) const
{
	BaseModel		*Parent;

	if( pParent.isValid() )
	{
		Parent = static_cast<BaseModel *>( pParent.internalPointer() );
	}
	else
	{
		Parent = mRootItem;
	}

	return( Parent->columnCount() );
}

QVariant ContextModel::data( const QModelIndex &index, int role ) const
{
	if( !index.isValid() )
	{
		return QVariant();
	}

	if( role != Qt::DisplayRole )
	{
		return QVariant();
	}

	BaseModel	*Item = static_cast<BaseModel *>( index.internalPointer() );

	return( Item->data( index.column() ) );
}
