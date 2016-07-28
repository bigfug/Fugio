#ifndef CONTEXTVIEW_H
#define CONTEXTVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointF>
#include <QSettings>
#include <QList>
#include <QMap>
#include <QSharedPointer>

#include <fugio/node_interface.h>
#include <fugio/edit_interface.h>

#include "nodeitem.h"
#include "pinitem.h"
#include "contextprivate.h"
#include "model/contextmodel.h"

//#define USE_CONTEXT_MODEL

class QGestureEvent;
class QPanGesture;
class QPinchGesture;

class ContextWidgetPrivate;
class NoteItem;

typedef QPair<QPointF,QPointF>					MoveData;
typedef QMap<QUuid,MoveData>					NodeMoveData;
typedef QMap<QSharedPointer<NoteItem>,MoveData>	NoteMoveData;

class ContextView : public QGraphicsView, public fugio::EditInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::EditInterface )
	Q_PROPERTY( QBrush LabelBrush READ labelBrush WRITE setLabelBrush NOTIFY labelBrushUpdated )
	Q_PROPERTY( QFont  LabelFont READ labelFont WRITE setLabelFont NOTIFY labelFontUpdated )
	Q_PROPERTY( QFont  pin_font READ pinFont WRITE setPinFont NOTIFY pinFontUpdated )
	Q_PROPERTY( QString NoteFontFace READ noteFontFace WRITE setNoteFontFace NOTIFY noteFontFaceUpdated )
	Q_PROPERTY( int     NoteFontSize READ noteFontSize WRITE setNoteFontSize NOTIFY noteFontSizeUpdated )
	Q_PROPERTY( QColor  NoteColour READ noteColour WRITE setNoteColour NOTIFY noteColourUpdated )
	Q_PROPERTY( QUuid	GroupId READ groupId WRITE setGroupId NOTIFY groupIdChanged )
	Q_PROPERTY( QPointF PastePoint READ PastePoint WRITE setPastePoint )

public:
	explicit ContextView( QWidget *pParent = 0 );

	virtual ~ContextView( void );

	void setContext( QSharedPointer<fugio::ContextInterface> pContext );

	static inline int gridSize( void )
	{
		return( 20 );
	}

	inline QSharedPointer<fugio::ContextInterface> context( void )
	{
		return( mContext );
	}

	void setChanged( bool pChanged = true )
	{
		mChanged = pChanged;
	}

	ContextWidgetPrivate *widget( void );

	QBrush labelBrush( void ) const
	{
		return( mLabelBrush );
	}

	QFont labelFont() const
	{
		return m_LabelFont;
	}

	QFont pinFont() const
	{
		return m_pin_font;
	}

	void setNodePositionFlag( void )
	{
		mNodePositionFlag = true;
	}

	QSharedPointer<NodeItem> findNodeItem( const QUuid &pId )
	{
		return( mNodeList.value( pId ) );
	}

	QString noteFontFace() const
	{
		return m_NoteFontFace;
	}

	int noteFontSize() const
	{
		return m_NoteFontSize;
	}

	QColor noteColour() const
	{
		return m_NoteColour;
	}

	void clearPasteOffset( void )
	{
		mPasteOffset = 0;
	}

	void resetPasteOffset( void )
	{
		mPasteOffset = gridSize();
	}

	void decreasePasteOffset( void )
	{
		mPasteOffset = std::max( mPasteOffset - gridSize(), gridSize() );
	}

	void increasePasteOffset( void )
	{
		mPasteOffset += gridSize();
	}

	void clearPasteNodes( void )
	{
		mPasteNodes.clear();
	}

	QList<QUuid> pasteNodes( void )
	{
		return( mPasteNodes );
	}

	bool undoNodeUpdates( void ) const
	{
		return( mUndoNodeUpdates );
	}

	void setUndoNodeUpdates( bool pUpdates )
	{
		mUndoNodeUpdates = pUpdates;
	}

	QUuid groupId() const
	{
		return m_GroupId;
	}

	QPointF PastePoint() const
	{
		return m_PastePoint;
	}

	typedef enum
	{
		TO_LEFT,
		TO_RIGHT,
		TO_CENTER
	} NodeMoveRelation;

	void moveNodeRelative( NodeItem *pNode, const QList<NodeItem *> &pNodeList, const QList<NoteItem *> &pNoteList, NodeMoveRelation pRelation );

signals:
//	void nodeInspection( NodeItem *pNodeItem );

	void labelBrushUpdated( QBrush );

	void labelFontUpdated(QFont arg);

	void pinFontUpdated(QFont arg);

	void noteFontFaceUpdated(QString arg);

	void noteFontSizeUpdated(int arg);

	void noteColourUpdated(QColor arg);

	void groupIdChanged(QUuid GroupId);

public slots:
	void setLabelBrush(QBrush arg)
	{
		if( mLabelBrush == arg )
		{
			return;
		}

		mLabelBrush = arg;

		emit labelBrushUpdated( arg );
	}

	void setSelectedColour( const QColor &pColor );

	virtual void cut( void ) Q_DECL_OVERRIDE;

	virtual void copy( void ) Q_DECL_OVERRIDE;

	virtual void saveSelectedTo( const QString &pFileName );

	virtual void paste( void ) Q_DECL_OVERRIDE;

	void nodeMoved( const QUuid &pUuid, const QPointF &P1, const QPointF &P2 );
	void noteMoved( NoteItem *pNoteItem, const QPointF &P1, const QPointF &P2 );

	void nodeMoveStarted( void );
	void nodeMoveFinished( void );

	void noteAdd( QSharedPointer<NoteItem> pNoteItem );
	void noteRemove( QSharedPointer<NoteItem> pNoteItem );

	virtual void groupSelected( void );

	void ungroupSelected( void );

	void groupParent( void );

	void pushGroup( const QUuid &pGroupId );

	void popGroup( void );

	QUuid group( const QString &pGroupName, QList<NodeItem *> &pNodeList, QList<NodeItem *> &pGroupList, QList<NoteItem *> &pNoteList, const QUuid &pGroupId = QUuid() );

	void ungroup( QList<NodeItem *> &pNodeList, QList<NodeItem *> &pGroupList, QList<NoteItem *> &pNoteList, const QUuid &pGroupId );

	void ungroup( NodeItem *pGroup );

protected:
	virtual void keyReleaseEvent( QKeyEvent *pEvent ) Q_DECL_OVERRIDE;

	void sortSelectedItems( QList<NodeItem *> &pNodeList, QList<NodeItem *> &pGroupList, QList<LinkItem *> &pLinkList, QList<NoteItem *> &pNoteList, bool pRecurse );

	bool itemsForRemoval( QList<NodeItem *> &pNodeItemList, QList<NodeItem *> &pGroupList, QList<LinkItem *> &pLinkItemList, QList<NoteItem *> &pNoteItemList, QList<QSharedPointer<fugio::NodeInterface>> &NodeList, QList<QSharedPointer<NodeItem>> &GroupList, QMultiMap<QUuid,QUuid> &LinkList, QList<QSharedPointer<NoteItem>> &NoteList );

	//bool itemsForRemoval( QList<QSharedPointer<fugio::NodeInterface> > &pNodeList, QMultiMap<QUuid, QUuid> &pLinkList, QList<QSharedPointer<NodeItem>> &pGroupList, QList<QSharedPointer<NoteItem> > &pNoteList );

	virtual void mouseDoubleClickEvent( QMouseEvent *pEvent ) Q_DECL_OVERRIDE;

	static QList<QUuid> nodeItemIds( const QList<NodeItem *> &pNodeList );

	static QList<QUuid> noteItemIds( const QList<NoteItem *> &pNoteList );

	QList<NodeItem *> nodesFromIds( const QList<QUuid> &pIdsLst ) const;

	QList<QUuid> recursiveGroupIds( const QList<NodeItem *> &pGroupList ) const;

	void itemsInGroups( const QList<QUuid> &pGroupIdList, QList<NodeItem *> &pNodeList ) const;

	void notesInGroups( const QList<QUuid> &pGroupIdList, QList<NoteItem *> &pNoteList ) const;

	void processGroupLinks(QSharedPointer<NodeItem> NI );

	void updatePastePoint(QList<NodeItem *> NodeItemList, QList<NoteItem *> NoteItemList);

	QMap<QUuid,QUuid> nodeGroups( QList<NodeItem *> pNodeList );

protected slots:
	void loadStarted( QSettings &pSettings, bool pPartial );
	void loadContext( QSettings &pSettings, bool pPartial );
	void loadEnded( QSettings &pSettings, bool pPartial );

	void saveContext( QSettings &pSettings ) const;

	void clearContext( void );

	void nodeAdded( QUuid pNodeId, QUuid pOrigId );
	void nodeRemoved( QUuid pNodeId );
	void nodeRenamed( QUuid pNodeId1, QUuid pNodeId2 );
	void nodeChanged( QUuid pNodeId );
	void nodeActivated( QUuid pNodeId );

	void pinAdded( QUuid pNodeId, QUuid pPinId );
	void pinRenamed( QUuid pNodeId, QUuid pPinId1, QUuid pPinId2 );
	void pinRemoved( QUuid pNodeId, QUuid pPinId );

	void linkAdded( QUuid pPinId1, QUuid pPinId2 );
	void linkRemoved( QUuid pPinId1, QUuid pPinId2 );

	void showWizard( void );

	void zoom(qreal factor, QPointF centerPoint);

protected:
	virtual void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
	virtual void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;
	virtual void dragMoveEvent(QDragMoveEvent *) Q_DECL_OVERRIDE;

	virtual void focusInEvent(QFocusEvent *) Q_DECL_OVERRIDE;
	virtual void focusOutEvent(QFocusEvent *) Q_DECL_OVERRIDE;

	virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

	virtual void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;

	bool gestureEvent( QGestureEvent *pEvent );

	void panTriggered( QPanGesture *pGesture );
	void pinchTriggered( QPinchGesture *pGesture );

	bool isGroupEmpty( const QUuid &pGroupId ) const;

	void clearTempLists( void );

public slots:
	void updateItemVisibility();

	// QObject interface
public:
	virtual bool event( QEvent * ) Q_DECL_OVERRIDE;


	void nodeAdd( QSharedPointer<NodeItem> Node );
	void nodeRemove( QSharedPointer<NodeItem> Node );

public slots:
	QSharedPointer<NoteItem> noteAdd(const QString &pText , QUuid pUuid);

	void setLabelFont(QFont arg)
	{
		if (m_LabelFont == arg)
			return;

		m_LabelFont = arg;
		emit labelFontUpdated(arg);
	}

	void setPinFont(QFont arg)
	{
		if (m_pin_font == arg)
			return;

		m_pin_font = arg;
		emit pinFontUpdated( arg );
	}

	void setNoteFontFace(QString arg)
	{
		if (m_NoteFontFace == arg)
			return;

		m_NoteFontFace = arg;
		emit noteFontFaceUpdated(arg);
	}

	void setNoteFontSize(int arg)
	{
		if (m_NoteFontSize == arg)
			return;

		m_NoteFontSize = arg;
		emit noteFontSizeUpdated(arg);
	}

	void setNoteColour(QColor arg)
	{
		if (m_NoteColour == arg)
			return;

		m_NoteColour = arg;
		emit noteColourUpdated(arg);
	}

	void setGroupId(QUuid GroupId);

	void setPastePoint(QPointF PastePoint)
	{
		if (m_PastePoint == PastePoint)
			return;

		m_PastePoint = PastePoint;
	}

private:
	typedef struct GroupStateEntry
	{
		QUuid		mGroupId;
		QTransform	mViewTransform;

		GroupStateEntry( QUuid pGroupId, QTransform pViewTransform )
			: mGroupId( pGroupId ), mViewTransform( pViewTransform )
		{

		}
	} GroupStateEntry;

	QGraphicsScene							 mContextScene;

#if defined( CONTEXT_MODEL )
	ContextModel							 mContextModel;
#endif

	QSharedPointer<fugio::ContextInterface>	 mContext;
	QMap<QUuid,QPointF>						 mPositions;
	QMap<QUuid,QPointF>						 mPastePositions;
	QMap<QUuid, QSharedPointer<NodeItem>>	 mNodeList;
	QList<QSharedPointer<NoteItem>>			 mNoteList;
	QList<QUuid>							 mGroupIds;
	QList<QUuid>							 mGroupStack;
	bool									 mChanged;
	QBrush									 mLabelBrush;
	QFont									 m_LabelFont;
	QFont									 m_pin_font;
	bool									 mNodePositionFlag;
	QPointF									 mNodePosition;
	QByteArray								 mPasteData;
	int										 mPasteOffset;
	QList<QUuid>							 mPasteNodes;
	bool									 mSaveOnlySelected;
	bool									 mUndoNodeUpdates;
	int										 mNodeMoveUndoId;

	NodeMoveData							 mNodeMoveData;
	NoteMoveData							 mNoteMoveData;

	// QWidget interface
	QString									 m_NoteFontFace;
	int										 m_NoteFontSize;
	QColor									 m_NoteColour;

	static bool								 mShownWizard;

	QPointF									 m_PastePoint;

	QUuid									 m_GroupId;

	QList<NodeItem *>						 mNodeItemList;
	QList<NodeItem *>						 mGroupItemList;
	QList<LinkItem *>						 mLinkItemList;
	QList<NoteItem *>						 mNoteItemList;
};

#endif // CONTEXTVIEW_H
