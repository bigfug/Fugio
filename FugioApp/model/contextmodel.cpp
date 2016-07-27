#include "contextmodel.h"

#include <fugio/context_signals.h>

#include <fugio/context_interface.h>

#include "basemodel.h"
#include "pinmodel.h"
#include "notemodel.h"

#include "app.h"
#include "mainwindow.h"

#include <QTreeView>
#include <QDockWidget>

ContextModel::ContextModel( QObject *pParent )
	: QAbstractItemModel( pParent )
{
	mRootItem = new GroupModel();
}

ContextModel::~ContextModel()
{
	clearContext();
}

void ContextModel::setContext( QSharedPointer<fugio::ContextInterface> pContext )
{
	clearContext();

	mContext = pContext;

	fugio::ContextSignals		*CS = mContext->qobject();

	connect( CS, SIGNAL(clearContext()), this, SLOT(clearContext()) );

	connect( CS, SIGNAL(nodeAdded(QUuid,QUuid)), this, SLOT(nodeAdded(QUuid,QUuid)) );
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

QUuid ContextModel::createGroup( const QUuid &pGroupId )
{
	GroupModel			*Parent = ( mCurrentGroup.isNull() ? mRootItem : mGroupMap.value( mCurrentGroup ) );

	if( !Parent )
	{
		return( QUuid() );
	}

	GroupModel			*Group = new GroupModel( Parent );

	if( !Group )
	{
		return( QUuid() );
	}

	QUuid				 GroupId = pGroupId.isNull() ? QUuid::createUuid() : pGroupId;

	int					 GroupIdx = Parent->rowCount();

	beginInsertRows( createIndex( Parent->row(), 0, Parent ), GroupIdx, GroupIdx );

	Parent->appendChild( Group );

	mGroupMap.insert( GroupId, Group );

	endInsertRows();

	emit groupAdded( GroupId );

	return( GroupId );
}

void ContextModel::removeGroup( const QUuid &pGroupId )
{
	GroupModel			*Group = mGroupMap.value( pGroupId );

	if( Group )
	{
		GroupModel		*Parent = Group->parent();
		int				 GroupIndex = Group->row();

		beginRemoveRows( createIndex( Parent->row(), 0, Parent ), GroupIndex, GroupIndex );

		Parent->removeChild( GroupIndex );

		mGroupMap.remove( pGroupId );

		endRemoveRows();

		delete Group;
	}
}

void ContextModel::removeNote( const QUuid &pNoteId )
{
	NoteModel			*Note = mNoteMap.value( pNoteId );

	if( Note )
	{
		GroupModel		*Parent = Note->parent();
		int				 NoteIndex = Note->row();

		beginRemoveRows( createIndex( Parent->row(), 0, Parent ), NoteIndex, NoteIndex );

		Parent->removeChild( NoteIndex );

		mNoteMap.remove( pNoteId );

		endRemoveRows();

		delete Note;
	}
}

void ContextModel::moveToGroup( const QUuid &pGroupId, const QList<QUuid> &pNodeList, const QList<QUuid> &pGroupList, const QList<QUuid> &pNoteList)
{
	GroupModel			*GroupDest = mGroupMap.value( pGroupId );

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

		GroupModel		*GroupSource = Node->parent();

		if( !GroupSource || GroupSource == GroupDest )
		{
			continue;
		}

		const int		NodeRow = Node->row();

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), NodeRow, NodeRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount() );

		GroupSource->removeChild( NodeRow );

		GroupDest->appendChild( Node );

		Node->setParent( GroupDest );

		endMoveRows();
	}

	for( const QUuid &Id : pGroupList )
	{
		GroupModel		*Group = mGroupMap.value( Id );

		if( !Group )
		{
			continue;
		}

		GroupModel		*GroupSource = Group->parent();

		if( !GroupSource || GroupSource == GroupDest )
		{
			continue;
		}

		const int		GroupRow = Group->row();

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), GroupRow, GroupRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount() );

		GroupSource->removeChild( GroupRow );

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

		GroupModel		*GroupSource = Note->parent();

		if( !GroupSource || GroupSource == GroupDest )
		{
			continue;
		}

		const int		NoteRow = Note->row();

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), NoteRow, NoteRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount() );

		GroupSource->removeChild( NoteRow );

		GroupDest->appendChild( Note );

		Note->setParent( GroupDest );

		endMoveRows();
	}

	emit layoutChanged();
}

QUuid ContextModel::createNote( void )
{
	GroupModel			*Parent = ( mCurrentGroup.isNull() ? mRootItem : mGroupMap.value( mCurrentGroup ) );

	if( !Parent )
	{
		return( QUuid() );
	}

	NoteModel			*Note = new NoteModel( Parent );

	if( !Note )
	{
		return( QUuid() );
	}

	QUuid				 NoteId = QUuid::createUuid();

	int					 NoteIdx = Parent->rowCount();

	beginInsertRows( createIndex( Parent->row(), 0, Parent ), NoteIdx, NoteIdx );

	Parent->appendChild( Note );

	endInsertRows();

	emit noteAdded( NoteId );

	return( NoteId );
}

QModelIndex ContextModel::nodeIndex( const QUuid &pId ) const
{
	NodeModel		*Node = mNodeMap.value( pId );

	return( Node ? createIndex( Node->row(), 0, Node ) : QModelIndex() );
}

QModelIndex ContextModel::groupIndex( const QUuid &pId ) const
{
	GroupModel		*Group = mGroupMap.value( pId );

	return( Group ? createIndex( Group->row(), 0, Group ) : QModelIndex() );
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
	emit layoutAboutToBeChanged();
}

void ContextModel::loadEnded( QSettings &pSettings, bool pPartial )
{
	changePersistentIndex( createIndex( 0, 0, mRootItem ), createIndex( mRootItem->rowCount(), 0, mRootItem ) );

	emit layoutChanged();
}

//-----------------------------------------------------------------------------
// fugio::ContextSignals

void ContextModel::clearContext()
{
	beginResetModel();

	mNodeMap.clear();

	mGroupMap.clear();

	mNoteMap.clear();

	mPinMap.clear();

	if( mRootItem )
	{
		delete mRootItem;
	}

	mRootItem = new GroupModel();

	endResetModel();
}

void ContextModel::nodeAdded( QUuid pNodeId, QUuid /* pOrigId */ )
{
	GroupModel		*Parent = ( mCurrentGroup.isNull() ? mRootItem : mGroupMap.value( mCurrentGroup ) );

	qDebug() << "nodeAdded" << pNodeId;

	const int		 ChildCount = Parent->rowCount();

	NodeModel		*Node = new NodeModel( pNodeId, Parent );

	if( Node )
	{
		beginInsertRows( createIndex( Parent->row(), 0, Parent ), ChildCount, ChildCount );

		mRootItem->appendChild( Node );

		endInsertRows();

		mNodeMap.insert( pNodeId, Node );
	}

	emit layoutChanged();
}

void ContextModel::nodeRemoved( QUuid pNodeId )
{
	NodeModel		*Node = mNodeMap.value( pNodeId );

	if( Node )
	{
		GroupModel		*Parent = Node->parent();

		if( Parent )
		{
			const int		 Index = Parent->childRow( Node );

			beginRemoveRows( createIndex( Parent->row(), 0, Parent ), Index, Index );

			Parent->removeChild( Index );

			endRemoveRows();
		}

		mNodeMap.remove( pNodeId );

		delete Node;
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
		QSharedPointer<fugio::PinInterface>	Pin = mContext->findPin( pPinId );

		if( Pin )
		{
			PinListModel	*PinList = ( Pin->direction() == PIN_INPUT ? Node->inputs() : Node->outputs() );

			beginInsertRows( createIndex( PinList->row(), 0, PinList ), PinList->rowCount(), PinList->rowCount() );

			PinList->appendPinId( pPinId );

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

	GroupModel		*GroupParent = dynamic_cast<GroupModel *>( Parent );

	if( GroupParent )
	{
		BaseModel		*Child = GroupParent->childAt( row );

		if( Child )
		{
			return( createIndex( row, column, Child ) );
		}

		return( QModelIndex() );
	}

	NodeModel		*NodeParent = dynamic_cast<NodeModel *>( Parent );

	if( NodeParent )
	{
		PinListModel		*Child = ( !row ? NodeParent->inputs() : NodeParent->outputs() );

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

	GroupModel		*Parent;

	if( pParent.isValid() )
	{
		Parent = static_cast<GroupModel *>( pParent.internalPointer() );
	}
	else
	{
		Parent = mRootItem;
	}

	return( Parent->rowCount() );
}

int ContextModel::columnCount( const QModelIndex &pParent ) const
{
	GroupModel		*Parent;

	if( pParent.isValid() )
	{
		Parent = static_cast<GroupModel *>( pParent.internalPointer() );
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
