#ifndef CONTEXTMODEL_H
#define CONTEXTMODEL_H

#include <QAbstractItemModel>

#include <fugio/context_interface.h>

#include "groupmodel.h"
#include "nodemodel.h"
#include "notemodel.h"

class ContextModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	ContextModel( QObject *pParent = nullptr );

	virtual ~ContextModel( void );

	void setContext( QSharedPointer<fugio::ContextInterface> pContext );

	QUuid createGroup( const QUuid &pParentId );
	QUuid createNote( const QUuid &pParentId );

	void removeGroup( const QUuid &pGroupId );
	void removeNote( const QUuid &pNoteId );

	void moveToGroup( const QUuid &pGroupId, const QList<QUuid> &pNodeList, const QList<QUuid> &pGroupList, const QList<QUuid> &pNoteList );

	QModelIndex nodeIndex( const QUuid &pId ) const;
	QModelIndex groupIndex( const QUuid &pId ) const;
	QModelIndex noteIndex( const QUuid &pId ) const;
	QModelIndex pinIndex( const QUuid &pId ) const;

signals:
	void groupAdded( const QUuid &pUuid );
	void noteAdded( const QUuid &pUuid );

public slots:

private slots:
	void clearContext( void );

	void nodeAdded( QUuid pNodeId, QUuid pOrigId );
	void nodeRemoved( QUuid pNodeId );
	void nodeRenamed( QUuid pNodeId, QUuid pOrigId );

	void pinAdded( QUuid pNodeId, QUuid pPinId );
	void pinRemoved( QUuid pNodeId, QUuid pPinId );
	void pinRenamed( QUuid pNewId, QUuid pOldId );

	// QAbstractItemModel interface
public:
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
	virtual QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
	virtual int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
	virtual int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
	virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::ContextInterface>		 mContext;
	GroupModel									*mRootItem;
	QMap<QUuid,NodeModel *>						 mNodeMap;
	QMap<QUuid,GroupModel *>					 mGroupMap;
	QMap<QUuid,PinModel *>						 mPinMap;
	QMap<QUuid,NoteModel *>						 mNoteMap;
};

#endif // CONTEXTMODEL_H
