#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsObject>
#include <QList>
#include <QUuid>
#include <QSharedPointer>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN
class NodeInterface;
class PinInterface;
FUGIO_NAMESPACE_END

class QGraphicsLinearLayout;
class QGraphicsScene;
class QGraphicsSimpleTextItem;
class QGraphicsProxyWidget;
class ContextView;
class PinItem;

class NodeItem : public QGraphicsObject
{
	Q_OBJECT

public:
	explicit NodeItem( ContextView *pContextView );

	explicit NodeItem( ContextView *pContextView, QUuid pNodeId, const QPointF &pPosition );

	virtual ~NodeItem( void );

	enum { Type = UserType + 1 };

	int type( void ) const
	{
		// Enable the use of qgraphicsitem_cast with this item.
		return Type;
	}

	inline QUuid id( void ) const
	{
		return( mNodeId );
	}

	inline QString name( void ) const
	{
		return( mNodeName );
	}

	inline QColor colour( void ) const
	{
		return( mBackgroundColour );
	}

	void setNodeId( const QUuid &pNodeId );

	bool hasPinsInGroup( void ) const;

//	inline QList< QSharedPointer<PinItem> > &pinItems( void )
//	{
//		return( mPinItems );
//	}

//	void pinInputAdd( const QString &pName, const QUuid &pId );
//	void pinOutputAdd( const QString &pName, const QUuid &pId );

	void pinInputRemove( const QUuid &pId );
	void pinOutputRemove( const QUuid &pId );

	PinItem *pinInputAdd( QSharedPointer<fugio::PinInterface> pPin );

	PinItem *pinOutputAdd( QSharedPointer<fugio::PinInterface> pPin );

	void pinInputRemove( QSharedPointer<fugio::PinInterface> pPin );

	void pinOutputRemove( QSharedPointer<fugio::PinInterface> pPin );

	void layoutPins( void );

	void updateGui( void );

	PinItem *findPinInput( QSharedPointer<fugio::PinInterface> pPin );

	PinItem *findPinOutput( QSharedPointer<fugio::PinInterface> pPin );

	virtual QRectF boundingRect( void ) const;

	void updateToolTip( void );

	QUuid groupId( void ) const;

	void setGroupId( const QUuid &pGroupId );

	PinItem *findPinInput( const QUuid &pId );
	PinItem *findPinOutput( const QUuid &pId );

	static QPointF computeTopLeftGridPoint(const QPointF &pointP);

	QList<PinItem *> inputs( void );

	QList<PinItem *> outputs( void );

public slots:
	void labelDrag( const QPointF &pMovement );

	void updateActiveState( void );

	void updateActiveState( const QBrush &pBrush );

	void updateLabelFont( const QFont &pFont );

	void updatePinFont( const QFont &pFont );

	void setColour( const QColor &pColor );

	void setNodePos( const QPointF &pPos );

	void setNodePosSilent( const QPointF &pPos );

	void setName( const QString &pName );

	void moveStarted( void );

protected:
	typedef QPair<QGraphicsSimpleTextItem *, PinItem *>		PinPair;

	PinPair *findPinPair( QSharedPointer<fugio::PinInterface> pPin );
	PinPair *findPinPairInput( const QUuid &pId );
	PinPair *findPinPairOutput( const QUuid &pId );
	int findPinPairInputIndex( const QUuid &pId ) const;
	int findPinPairOutputIndex( const QUuid &pId ) const;

	virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget = 0 );

	virtual QVariant itemChange( GraphicsItemChange pChange, const QVariant &pValue );

	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );

	virtual void mousePressEvent( QGraphicsSceneMouseEvent *pEvent );

	virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent );

	void pinLinkMove( const QList<NodeItem::PinPair> &pPinPairs, const QPointF &pMovement );

	void create( const QPointF &pPosition = QPointF() );

	static qreal pinLabelWidth( const QList<PinPair> &pPinPairs );

	static bool comparePinPair( const PinPair &P1, const PinPair &P2 );

	static QString helpUrl( QSharedPointer<fugio::NodeInterface> NODE );

protected slots:
	void pinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );

	void pinNameChanged( QSharedPointer<fugio::PinInterface> pPin );

	void pinsPaired( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 );
	void pinsUnpaired( QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2 );

	void nodeStatusUpdated( void );

	void menuRename( void );
	void menuActive( bool pActive );
	void menuSetColour( void );
	void menuAddInputPin( void );
	void menuAddOutputPin( void );
	void menuDelete( void );
	void menuHelp( void );
	void menuUngroup( void );

private:
	ContextView							*mContextView;

	QGraphicsRectItem					*mBorderItem;
	QGraphicsRectItem					*mLabelItem;
	QGraphicsSimpleTextItem				*mLabelText;
	QGraphicsRectItem					*mPinsItem;
	QGraphicsProxyWidget				*mNodeGui;
	QGraphicsEllipseItem				*mStatusItem;

	QList<PinPair>						 mInputs;
	QList<PinPair>						 mOutputs;

	QUuid								 mNodeId;
	QString								 mNodeName;
	QRectF								 mBounds;
	QColor								 mBackgroundColour;
	QPointF								 mMouseStartPosition;
	QPointF								 mMouseStartOffset;

	QPointF								 mOffset;
	QPointF								 mMoveStart;
	int									 mUndoId;

	QUuid								 mGroupId;
};

#endif // NODEITEM_H
