#ifndef LINKITEM_H
#define LINKITEM_H

#include <QGraphicsPathItem>
#include <QGraphicsWidget>

class PinItem;
class ContextView;

class LinkItem : public QGraphicsPathItem
{
public:
	explicit LinkItem( QGraphicsItem *pParent = 0 );

	virtual ~LinkItem( void );

	inline PinItem *srcPin( void ) const
	{
		return( mSrcPin );
	}

	inline PinItem *dstPin( void ) const
	{
		return( mDstPin );
	}

	void setSrcPin( PinItem *pSrcPin );
	void setDstPin( PinItem *pDstPin );

	void setDstPnt( const QPointF &P );

	enum { Type = UserType + 3 };

	int type( void ) const
	{
		// Enable the use of qgraphicsitem_cast with this item.
		return Type;
	}

	void updateSrcPin( void );
	void updateDstPin( void );

	virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget = 0 );

	inline QColor colour( void ) const
	{
		return( mColour );
	}

public slots:
	void setColour( const QColor &pColor );

	void setStatusColour( const QColor &pColour );

	void clrStatusColour( void );

protected:
	void updatePath( void );

	void updateColour( void );

	void updateToolTip( void );

	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent );

private:
	QPointF						 P1, P2;
	PinItem						*mSrcPin;
	PinItem						*mDstPin;
	QColor						 mStatusColour;
	QColor						 mColour;
};

#endif // LINKITEM_H
