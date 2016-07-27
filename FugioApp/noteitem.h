#ifndef NOTEITEM_H
#define NOTEITEM_H

#include <QGraphicsTextItem>
#include <QUuid>
#include <QColor>
#include <QPointF>

class QGraphicsTextItem;
class ContextView;

class NoteItem : public QGraphicsTextItem
{
public:
	NoteItem( ContextView *pContextView, const QUuid &pNoteId, const QString &pText );

	virtual ~NoteItem( void );

	inline QUuid id( void ) const
	{
		return( mNoteId );
	}

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
	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) Q_DECL_OVERRIDE;

	// QGraphicsItem interface
protected:
	virtual QVariant itemChange( GraphicsItemChange pChange, const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

protected:
	ContextView				*mContextView;
	QColor					 mBackgroundColour;
	QPointF					 mOffset;
	QPointF					 mMoveStart;

	QUuid					 mGroupId;
	QUuid					 mNoteId;
};

#endif // NOTEITEM_H
