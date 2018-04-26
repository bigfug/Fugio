#ifndef PINWIDGET_H
#define PINWIDGET_H

#include <QGraphicsObject>
#include <QMouseEvent>
#include <QTimer>

#include <memory>

#include <fugio/pin_interface.h>

class QGraphicsLineItem;
class ContextView;
class NodeItem;
class LinkItem;

class PinItem : public QGraphicsObject
{
	Q_OBJECT

public:
	explicit PinItem( ContextView *pContextView, QSharedPointer<fugio::PinInterface> pPin, NodeItem *pParent );

	explicit PinItem( ContextView *pContextView, const QUuid &pId, NodeItem *pParent );

	virtual ~PinItem( void );

	QSharedPointer<fugio::PinInterface> pin( void )
	{
		return( mPin );
	}

	virtual QRectF boundingRect( void ) const;

	virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget = 0 );

	enum { Type = UserType + 2 };

	int type( void ) const
	{
		// Enable the use of qgraphicsitem_cast with this item.
		return Type;
	}

	void linkMove( const QPointF &pMovement );

	void linkAdd( LinkItem *pLinkItem );
	void linkRem( LinkItem *pLinkItem );

	LinkItem *findLink( PinItem *pDst );

	bool hasLink( PinItem *pDst ) const;

	QUuid uuid( void ) const;

	inline QString name( void ) const
	{
		return( mPinName );
	}

	inline QColor colour( void ) const
	{
		return( mPinColour );
	}

	bool isGlobal( void ) const
	{
		return( mPin->setting( "global", false ).toBool() );
	}

	static QTimer &longPressTimer( void )
	{
		static QTimer	LongPressTimer;

		return( LongPressTimer );
	}

public slots:
	void setColour( const QColor &pColur );

	void setName( const QString &pName )
	{
		mPinName = pName;
	}

	void longPressTimeout( void );

protected:
	virtual void mousePressEvent( QGraphicsSceneMouseEvent *pEvent );

	virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent );

	virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent );

	virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );

	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );

	virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *pEvent );

	virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *pEvent );

	virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent );

	virtual void dropEvent( QGraphicsSceneDragDropEvent *pEvent );

private:
	PinItem *findDest( const QPointF &pPoint );

	QString helpUrl( void );

signals:
	void colourUpdated( const QColor &pColor );

	void globalUpdated( bool pGlobal );

protected slots:
	void menuRename( void );
	void menuEditDefault( void );
	void menuRemove( void );
	void menuHide( void );
	void menuUpdatable( void );
	void menuAlwaysUpdate( void );
	void menuImport( void );
	void menuExport( void );
	void menuJoin( const QString &pUuid );
	void menuSplit( const QString &pUuid );
	void menuSetColour( void );
	void menuAddGroupInput( void );
	void menuRemGroupInput( void );
	void menuAddGroupOutput( void );
	void menuRemGroupOutput( void );
	void menuMakeGlobal( void );
	void menuRemoveGlobal( void );
	void menuConnectGlobal( void );
	void menuDisconnectGlobal( void );
	void menuHelp();

private:
	ContextView								*mContextView;
	QSharedPointer<fugio::PinInterface>		 mPin;
	QList<LinkItem *>						 mLinks;
	std::unique_ptr<LinkItem>				 mLink;
	QUuid									 mPinId;
	QString									 mPinName;
	QColor									 mPinColour;
	QPointF									 mDragStartPoint;
};

#endif // PINWIDGET_H
