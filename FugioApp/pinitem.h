#ifndef PINWIDGET_H
#define PINWIDGET_H

#include <QGraphicsObject>
#include <QMouseEvent>

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

public slots:
	void setColour( const QColor &pColur );

	void setName( const QString &pName )
	{
		mPinName = pName;
	}

protected:
	virtual void mousePressEvent( QGraphicsSceneMouseEvent *pEvent );

	virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent );

	virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent );

	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );

	virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *pEvent );

	virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *pEvent );

	virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent );

	virtual void dropEvent( QGraphicsSceneDragDropEvent *pEvent );

private:
	PinItem *findDest( const QPointF &pPoint );

signals:
	void colourUpdated( const QColor &pColor );

protected slots:
	void menuRename( void );
	void menuEditDefault( void );
	void menuRemove( void );
	void menuHide( void );
	void menuUpdatable( void );
	void menuImport( void );
	void menuExport( void );
	void menuJoin( const QString &pUuid );
	void menuSplit( const QString &pUuid );
	void menuSetColour( void );
	void menuAddGroupInput( void );
	void menuRemGroupInput( void );
	void menuAddGroupOutput( void );
	void menuRemGroupOutput( void );

private:
	ContextView								*mContextView;
	QSharedPointer<fugio::PinInterface>		 mPin;
	QList<LinkItem *>						 mLinks;
	LinkItem								*mLink;
	QUuid									 mPinId;
	QString									 mPinName;
	QColor									 mPinColour;
};

#endif // PINWIDGET_H
