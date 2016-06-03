#include "noteitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QCursor>
#include <QDebug>

#include "contextview.h"

NoteItem::NoteItem( ContextView *pContextView, const QString &pText )
	: mContextView( pContextView )
{
	setPlainText( pText );

	setTextInteractionFlags( Qt::TextEditorInteraction );

	setOpenExternalLinks( true );

	setFlags( flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges );

	setFont( QFont( mContextView->noteFontFace(), mContextView->noteFontSize() ) );

	setBackgroundColour( mContextView->noteColour() );
}

NoteItem::~NoteItem( void )
{
}

QUuid NoteItem::groupId() const
{
	return( mGroupId );
}

void NoteItem::setGroupId(const QUuid &pGroupId)
{
	if( mGroupId != pGroupId )
	{
		mGroupId = pGroupId;

		mContextView->update();
	}
}

void NoteItem::moveStarted()
{
	mOffset = pos();// - computeTopLeftGridPoint( pos() );

	mMoveStart = pos();
}

void NoteItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	painter->fillRect( boundingRect(), mBackgroundColour );

	QGraphicsTextItem::paint( painter, option, widget );
}

QVariant NoteItem::itemChange( QGraphicsItem::GraphicsItemChange pChange, const QVariant &pValue )
{
	if( pChange == ItemPositionHasChanged )
	{
		mContextView->noteMoved( this, mMoveStart, pValue.toPointF() );
	}

	return QGraphicsItem::itemChange( pChange, pValue );
}

void NoteItem::mousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
	QGraphicsTextItem::mousePressEvent( pEvent );

	mContextView->nodeMoveStarted();
}

void NoteItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
	mContextView->nodeMoveFinished();

	QGraphicsTextItem::mouseReleaseEvent( pEvent );
}
