#include "contextmodel.h"

#include <fugio/context_signals.h>

#include <fugio/context_interface.h>

#include "basemodel.h"
#include "pinmodel.h"
#include "notemodel.h"

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

//	if( QSharedPointer<ContextPrivate> C = qSharedPointerCast<ContextPrivate>( mContext ) )
//	{
//		connect( C.data(), SIGNAL(loadStart(QSettings&,bool)), this, SLOT(loadStarted(QSettings&,bool)) );
//		connect( C.data(), SIGNAL(loading(QSettings&,bool)), this, SLOT(loadContext(QSettings&,bool)) );
//		connect( C.data(), SIGNAL(loadEnd(QSettings&,bool)), this, SLOT(loadEnded(QSettings&,bool)) );

//		connect( C.data(), SIGNAL(saving(QSettings&)), this, SLOT(saveContext(QSettings&)) );

//		connect( C.data(), SIGNAL(nodeUpdated(QUuid)), this, SLOT(nodeChanged(QUuid)) );
//		connect( C.data(), SIGNAL(nodeActivated(QUuid)), this, SLOT(nodeActivated(QUuid)) );

//		connect( C.data(), SIGNAL(linkAdded(QUuid,QUuid)), this, SLOT(linkAdded(QUuid,QUuid)) );
//		connect( C.data(), SIGNAL(linkRemoved(QUuid,QUuid)), this, SLOT(linkRemoved(QUuid,QUuid)) );
	//	}
}

void ContextModel::setCurrentGroup( const QUuid &pGroupId )
{
	mCurrentGroup = pGroupId;
}

//-----------------------------------------------------------------------------

QUuid ContextModel::createGroup( void )
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

	QUuid				 GroupId = QUuid::createUuid();

	int					 GroupIdx = Parent->rowCount( 0 );

	beginInsertRows( createIndex( 0, Parent->row(), Parent ), GroupIdx, GroupIdx );

	Parent->appendChild( Group );

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

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), NodeRow, NodeRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount( 0 ) );

		GroupSource->removeChild( NodeRow );

		GroupDest->appendChild( Node );

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

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), GroupRow, GroupRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount( 0 ) );

		GroupSource->removeChild( GroupRow );

		GroupDest->appendChild( Group );

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

		beginMoveRows( createIndex( GroupSource->row(), 0, GroupSource ), NoteRow, NoteRow, createIndex( GroupDest->row(), 0, GroupDest ), GroupDest->rowCount( 0 ) );

		GroupSource->removeChild( NoteRow );

		GroupDest->appendChild( Note );

		endMoveRows();
	}
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

	int					 NoteIdx = Parent->rowCount( 0 );

	beginInsertRows( createIndex( 0, Parent->row(), Parent ), NoteIdx, NoteIdx );

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

	const int		 ChildCount = Parent->rowCount( 0 );

	NodeModel		*Node = new NodeModel( pNodeId, Parent );

	if( Node )
	{
		beginInsertRows( QModelIndex(), ChildCount, ChildCount );

		mRootItem->appendChild( Node );

		endInsertRows();

		mNodeMap.insert( pNodeId, Node );
	}
}

void ContextModel::nodeRemoved( QUuid pNodeId )
{
	NodeModel		*Node = mNodeMap.value( pNodeId );

	if( Node )
	{
		const int		 Index = Node->parent()->childRow( Node );

		beginRemoveRows( createIndex( 0, 0, Node->parent() ), Index, Index );

		Node->parent()->removeChild( Index );

		endRemoveRows();

		mNodeMap.remove( pNodeId );

		delete Node;
	}
}

void ContextModel::nodeRenamed( QUuid pNodeId, QUuid pOrigId )
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
			if( Pin->direction() == PIN_INPUT )
			{
				beginInsertRows( createIndex( Node->parent()->childRow( Node ), 0, Node ), Node->inputCount(), Node->inputCount() );

				Node->addPinInput( pPinId );
			}
			else
			{
				beginInsertRows( createIndex( Node->parent()->childRow( Node ), 1, Node ), Node->outputCount(), Node->outputCount() );

				Node->addPinOutput( pPinId );
			}

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
		const int		PinRow = Pin->row();
		const int		PinCol = ( Pin->direction() == PIN_INPUT ? 0 : 1 );

		beginRemoveRows( createIndex( Node->row(), PinCol, Node ), PinRow, PinRow );

		Node->remPin( Pin );

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
	GroupModel		*Parent;

	if( parent.isValid() )
	{
		Parent = static_cast<GroupModel *>( parent.internalPointer() );
	}
	else
	{
		Parent = mRootItem;
	}

	BaseModel		*Child = Parent->childAt( row );

	return( Child ? createIndex( row, column, Child ) : QModelIndex() );
}

QModelIndex ContextModel::parent( const QModelIndex &pChildIndex ) const
{
	if( !pChildIndex.isValid() )
	{
		return( QModelIndex() );
	}

	BaseModel		*Child = static_cast<BaseModel *>( pChildIndex.internalPointer() );
	BaseModel		*Parent = Child->parent();

	if( Parent == mRootItem )
	{
		return( QModelIndex() );
	}

	return( createIndex( Parent->row(), 0, Parent ) );
}

int ContextModel::rowCount( const QModelIndex &pParent ) const
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

	return( Parent->rowCount( pParent.column() ) );
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
