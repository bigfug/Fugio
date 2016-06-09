#ifndef NOTEITEM_H
#define NOTEITEM_H

#include <QGraphicsTextItem>

class QGraphicsTextItem;
class ContextView;

class NoteItem : public QGraphicsTextItem
{
public:
	NoteItem( ContextView *pContextView, const QString &pText );

	virtual ~NoteItem( void );

	void setBackgroundColour( const QColor &pColour )
	{
		mBackgroundColour = pColour;
	}

	QColor backgroundColour( void ) const
	{
		return( mBackgroundColour );
	}

	ContextView *view( void )
	{
		return( mContextView );
	}

	QUuid groupId( void ) const;

	void setGroupId( const QUuid &pGroupId );

	void moveStarted( void );

	// QGraphicsItem interface
public:
	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

	// QGraphicsItem interface
protected:
	virtual QVariant itemChange( GraphicsItemChange pChange, const QVariant &pValue);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

protected:
	ContextView				*mContextView;
	QColor					 mBackgroundColour;
	QPointF					 mOffset;
	QPointF					 mMoveStart;

	QUuid								 mGroupId;
};

#endif // NOTEITEM_H
