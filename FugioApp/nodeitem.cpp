#include "nodeitem.h"

#include <QDebug>

#include <QGraphicsScene>

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QPainter>
#include <QColorDialog>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QMainWindow>
#include <QDesktopServices>
#include <QMetaClassInfo>

#include <qmath.h>

#include "app.h"
#include "mainwindow.h"

#include "pinitem.h"
#include "contextview.h"

#include "nodeprivate.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

#include <fugio/pin_interface.h>
#include <fugio/pin_signals.h>

#include "contextwidgetprivate.h"

#include "undo/cmdnoderename.h"
#include "undo/cmdnodesetcolour.h"
#include "undo/cmdnodemove.h"
#include "undo/cmdnodepinadd.h"
#include "undo/cmdnodesetactive.h"
#include "undo/cmdnoderemove.h"
#include "undo/cmdgrouprename.h"

//static const int		PIN_BORDER = 5;
//static const int		PIN_MARGIN = 20;
//static const int		PIN_WIDTH  = 10;
//static const int		PIN_HEIGHT = 10;

NodeItem::NodeItem( ContextView *pContextView )
	: mContextView( pContextView ), mNodeGui( 0 ), mStatusItem( 0 ), mBackgroundColour( QColor( Qt::cyan ).lighter( 180 ) ), mUndoId( 1 )
{
	pContextView->scene()->addItem( this );

	create();
}

NodeItem::NodeItem( ContextView *pContextView, QUuid pNodeId, const QPointF &pPosition )
	: mContextView( pContextView ), mNodeGui( 0 ), mStatusItem( 0 ), mNodeId( pNodeId ), mBackgroundColour( QColor( Qt::cyan ).lighter( 180 ) ), mUndoId( 1 )
{
	pContextView->scene()->addItem( this );

	create( pPosition );
}

NodeItem::~NodeItem( void )
{
	mInputs.clear();

	mOutputs.clear();

	if( scene() )
	{
		mContextView->scene()->removeItem( this );
	}

	//qDebug() << "~NodeItem" << mNodeId;
}

void NodeItem::setNodeId( const QUuid &pNodeId )
{
	mNodeId = pNodeId;

	updateActiveState();
}

bool NodeItem::hasPinsInGroup() const
{
	QSharedPointer<NodeItem>	GI = mContextView->findNodeItem( groupId() );

	if( !GI )
	{
		return( false );
	}

	for( const PinPair &PP : mInputs )
	{
		if( GI->findPinInput( PP.second->uuid() ) )
		{
			return( true );
		}
	}

	for( const PinPair &PP : mOutputs )
	{
		if( GI->findPinOutput( PP.second->uuid() ) )
		{
			return( true );
		}
	}

	return( false );
}

//void NodeItem::pinInputAdd( const QString &pName, const QUuid &pId )
//{
//	QGraphicsSimpleTextItem		*TextItem = new QGraphicsSimpleTextItem( pName, mPinsItem );
//	PinItem						*Pin = new PinItem( mContextView, pId, this );

//	TextItem->setFont( mContextView->pinFont() );

//	mInputs.append( PinPair( TextItem, Pin ) );

//	std::sort( mInputs.begin(), mInputs.end(), &NodeItem::comparePinPair );
//}

//void NodeItem::pinOutputAdd( const QString &pName, const QUuid &pId )
//{
//	QGraphicsSimpleTextItem		*TextItem = new QGraphicsSimpleTextItem( pName, mPinsItem );
//	PinItem						*Pin = new PinItem( mContextView, pId, this );

//	TextItem->setFont( mContextView->pinFont() );

//	mOutputs.append( PinPair( TextItem, Pin ) );

//	std::sort( mOutputs.begin(), mOutputs.end(), &NodeItem::comparePinPair );
//}

void NodeItem::pinInputRemove(const QUuid &pId)
{
	int				 PI = findPinPairInputIndex( pId );

	if( PI >= 0 )
	{
		PinPair		 PP = mInputs[ PI ];

		delete PP.first;
		delete PP.second;

		mInputs.removeAt( PI );
	}
}

void NodeItem::pinOutputRemove( const QUuid &pId )
{
	int				 PI = findPinPairOutputIndex( pId );

	if( PI >= 0 )
	{
		PinPair		 PP = mOutputs[ PI ];

		delete PP.first;
		delete PP.second;

		mOutputs.removeAt( PI );
	}
}

QRectF NodeItem::boundingRect( void ) const
{
	return( childrenBoundingRect() );
}

void NodeItem::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget )
{
	Q_UNUSED( pPainter )
	Q_UNUSED( pWidget )

//	QStyleOptionGraphicsItem myoption = (*pOption);

//	myoption.state &= !QStyle::State_Selected;

	if( pOption->state.testFlag( QStyle::State_Selected ) )
	{
		mBorderItem->setPen( QPen( Qt::cyan ) );
	}
	else
	{
		mBorderItem->setPen( QPen( Qt::lightGray ) );
	}
}

void NodeItem::mousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
	QGraphicsObject::mousePressEvent( pEvent );

	mContextView->nodeMoveStarted();
}


void NodeItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
	mContextView->nodeMoveFinished();

	QGraphicsObject::mouseReleaseEvent( pEvent );
}

QPointF NodeItem::computeTopLeftGridPoint( const QPointF &pPoint )
{
	qreal xV = qRound( pPoint.x() / ContextView::gridSize() ) * ContextView::gridSize();
	qreal yV = qRound( pPoint.y() / ContextView::gridSize() ) * ContextView::gridSize();

	return( QPointF( xV, yV ) );
}

QList<PinItem *> NodeItem::inputs()
{
	QList<PinItem *>	Pins;

	for( PinPair &PP : mInputs )
	{
		Pins << PP.second;
	}

	return( Pins );
}

QList<PinItem *> NodeItem::outputs()
{
	QList<PinItem *>	Pins;

	for( PinPair &PP : mOutputs )
	{
		Pins << PP.second;
	}

	return( Pins );
}

void NodeItem::pinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
{
	Q_UNUSED( pNode )

	for( int i = 0 ; i < mInputs.size() ; i++ )
	{
		PinPair		PP = mInputs.at( i );

		if( PP.second->pin() != pPin )
		{
			continue;
		}

		mInputs.removeAt( i );

		delete PP.first;
		delete PP.second;

		break;
	}

	for( int i = 0 ; i < mOutputs.size() ; i++ )
	{
		PinPair		PP = mOutputs.at( i );

		if( PP.second->pin() != pPin )
		{
			continue;
		}

		mOutputs.removeAt( i );

		delete PP.first;
		delete PP.second;

		break;
	}

	updateGui();
}

QVariant NodeItem::itemChange( QGraphicsItem::GraphicsItemChange pChange, const QVariant &pValue )
{
	if( pChange == ItemPositionChange )
	{
		QPointF closestPoint = computeTopLeftGridPoint( pValue.toPointF() );

//		qDebug() << "ItemPositionChange" << mNodeName << pos() << "->" << pValue.toPointF() << "->" << closestPoint;

		return( closestPoint + mOffset );
	}

	if( pChange == ItemPositionHasChanged )
	{
		//qDebug() << "ItemPositionHasChanged" << mNodeName << pValue.toPointF();

		mContextView->nodeMoved( mNodeId, mMoveStart, pValue.toPointF() );

		labelDrag( QPointF() );

		return( QVariant() );
	}

	return QGraphicsItem::itemChange( pChange, pValue );
}

QString NodeItem::helpUrl( QSharedPointer<fugio::NodeInterface> NODE )
{
	QString		HelpUrl;

	if( NODE && NODE->hasControl() )
	{
		const QMetaObject	*DMO = NODE->control()->qobject()->metaObject();

		if( DMO )
		{
			int			 HelpClassInfoIndex = DMO->indexOfClassInfo( "URL" );

			if( HelpClassInfoIndex != -1 )
			{
				HelpUrl = QString( DMO->classInfo( HelpClassInfoIndex ).value() );
			}
		}
	}

	return( HelpUrl );
}

void NodeItem::contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
	QSharedPointer<fugio::NodeInterface>		NODE = mContextView->context()->findNode( mNodeId );

	QString		HelpUrl = helpUrl( NODE );

	QMenu		 Menu;
	QAction		*Action;

	if( ( Action = Menu.addAction( NODE ? NODE->name() : mNodeName ) ) )
	{
		Action->setEnabled( false );

		Menu.addSeparator();
	}

	if( ( Action = Menu.addAction( tr( "Rename..." ), this, SLOT(menuRename()) ) ) && mNodeGui )
	{
		// Under 5.6.x renaming nodes with GUI's caused very odd position
		// jumping when the user renamed a node, then attempted to drag it
		// This seems to be fixed in 5.7.0

#if ( QT_VERSION < QT_VERSION_CHECK( 5, 7, 0 ) )
		Action->setEnabled( false );
#endif
	}

	Menu.addAction( tr( "Set Colour..." ), this, SLOT(menuSetColour()) );

	if( NODE && !NODE->control()->pinAddTypesInput().isEmpty() && ( Action = Menu.addAction( tr( "Add Input Pin..." ) ) ) )
	{
		connect( Action, SIGNAL(triggered()), this, SLOT(menuAddInputPin()) );
	}

	if( NODE && !NODE->control()->pinAddTypesOutput().isEmpty() && ( Action = Menu.addAction( tr( "Add Output Pin..." ) ) ) )
	{
		connect( Action, SIGNAL(triggered()), this, SLOT(menuAddOutputPin()) );
	}

	if( NODE && ( Action = Menu.addAction( tr( "Active" ) ) ) )
	{
		Action->setCheckable( true );
		Action->setChecked( NODE->isActive() );

		connect( Action, SIGNAL(toggled(bool)), this, SLOT(menuActive(bool)) );
	}

	if( ( Action = Menu.addAction( tr( "Delete" ) ) ) )
	{
		connect( Action, SIGNAL(triggered()), this, SLOT(menuDelete()) );
	}

	if( NODE && !HelpUrl.isEmpty() )
	{
		Menu.addSeparator();

		if( ( Action = Menu.addAction( tr( "Help..." ) ) ) )
		{
			connect( Action, SIGNAL(triggered()), this, SLOT(menuHelp()) );
		}
	}

	if( !NODE )
	{
		Menu.addAction( tr( "Ungroup" ), this, SLOT(menuUngroup()) );
	}

	Menu.exec( pEvent->screenPos() );
}

void NodeItem::labelDrag( const QPointF &pMovement )
{
	pinLinkMove( mInputs, pMovement );
	pinLinkMove( mOutputs, pMovement );
}

void NodeItem::pinLinkMove( const QList<NodeItem::PinPair> &pPinPairs, const QPointF &pMovement )
{
	for( int i = 0 ; i < pPinPairs.size() ; i++ )
	{
		PinPair		PP = pPinPairs.at( i );

		PP.second->linkMove( pMovement );
	}
}

void NodeItem::updateToolTip( void )
{
	QSharedPointer<fugio::NodeInterface>			NODE = mContextView->context()->findNode( mNodeId );

	QStringList		NodDat;

	if( NODE )
	{
		NodDat << QString( "<b>%1</b>" ).arg( NODE->name() );

		QString			NodTyp = gApp->global().nodeName( NODE->controlUuid() );

		if( !NodTyp.isEmpty() )
		{
			NodDat << NodTyp;
		}
		else
		{
			NodDat << NODE->controlUuid().toString();
		}

		const QMetaObject		*NodeMeta = gApp->global().findNodeMetaObject( NODE->controlUuid() );

		if( NodeMeta )
		{
			int		InfIdx;

			if( ( InfIdx = NodeMeta->indexOfClassInfo( "Description" ) ) != -1 )
			{
				QMetaClassInfo	InfInf = NodeMeta->classInfo( InfIdx );

				NodDat << InfInf.value();
			}
		}
	}
	else
	{
		NodDat << QString( "<b>%1</b>" ).arg( mNodeName );
	}

	for( QString &S : NodDat )
	{
		S.prepend( "<p>" );
		S.append( "</p>" );
	}

	mLabelItem->setToolTip( NodDat.join( "\n" ) );
}

QUuid NodeItem::groupId() const
{
	return( mGroupId );
}

void NodeItem::setGroupId(const QUuid &pGroupId)
{
	if( mGroupId != pGroupId )
	{
		mGroupId = pGroupId;

		mContextView->update();
	}
}

PinItem *NodeItem::findPinInput(const QUuid &pId)
{
	if( PinPair *PP = findPinPairInput( pId ) )
	{
		return( PP->second );
	}

	return( nullptr );
}

PinItem *NodeItem::findPinOutput(const QUuid &pId)
{
	if( PinPair *PP = findPinPairOutput( pId ) )
	{
		return( PP->second );
	}

	return( nullptr );
}

NodeItem::PinPair *NodeItem::findPinPairInput(const QUuid &pId)
{
	for( PinPair &PP : mInputs )
	{
		if( PP.second->uuid() == pId )
		{
			return( &PP );
		}
	}

	return( nullptr );
}

NodeItem::PinPair *NodeItem::findPinPairOutput(const QUuid &pId)
{
	for( PinPair &PP : mOutputs )
	{
		if( PP.second->uuid() == pId )
		{
			return( &PP );
		}
	}

	return( nullptr );
}

int NodeItem::findPinPairInputIndex(const QUuid &pId) const
{
	for( int i = 0 ; i < mInputs.size() ; i++ )
	{
		const PinPair &PP = mInputs.at( i );

		if( PP.second->uuid() == pId )
		{
			return( i );
		}
	}

	return( -1 );
}

int NodeItem::findPinPairOutputIndex(const QUuid &pId) const
{
	for( int i = 0 ; i < mOutputs.size() ; i++ )
	{
		const PinPair &PP = mOutputs.at( i );

		if( PP.second->uuid() == pId )
		{
			return( i );
		}
	}

	return( -1 );
}

void NodeItem::create( const QPointF &pPosition )
{
	QSharedPointer<fugio::NodeInterface>			NODE = mContextView->context()->findNode( mNodeId );

	connect( mContextView, SIGNAL(labelBrushUpdated(QBrush)), this, SLOT(updateActiveState(QBrush)) );

	connect( mContextView, SIGNAL(labelFontUpdated(QFont)), this, SLOT(updateLabelFont(QFont)) );

	connect( mContextView, SIGNAL(pinFontUpdated(QFont)), this, SLOT(updatePinFont(QFont)) );

	if( NODE )
	{
		connect( NODE->qobject(), SIGNAL(pinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );

		connect( NODE->qobject(), SIGNAL(statusUpdated()), this, SLOT(nodeStatusUpdated()) );

		connect( NODE->qobject(), SIGNAL(pinsPaired(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinsPaired(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );
		connect( NODE->qobject(), SIGNAL(pinsUnpaired(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinsUnpaired(QSharedPointer<fugio::PinInterface>,QSharedPointer<fugio::PinInterface>)) );

		mBackgroundColour = QColor( NODE->setting( "editor/colour", mBackgroundColour.name() ).toString() );
	}

	mBorderItem = new QGraphicsRectItem( this );

	//mBorderItem->setPen( QPen( Qt::lightGray ) );

	mLabelItem = new QGraphicsRectItem( mBorderItem );

	//mLabelItem->setPen( QPen( Qt::lightGray ) );
	mLabelItem->setBrush( mContextView->labelBrush() );

	updateActiveState();

	mNodeName = NODE ? NODE->name() : "[GROUP]";

	mLabelText = new QGraphicsSimpleTextItem( mNodeName, mLabelItem );

	mLabelText->setPos( 5, 5 );
	mLabelText->setFont( mContextView->labelFont() );

	if( NODE )
	{
		mStatusItem = new QGraphicsEllipseItem( 0, 0, 10, 10, mLabelItem );

		mStatusItem->setBrush( Qt::red );
		mStatusItem->setToolTip( "Status" );
	}

	mPinsItem = new QGraphicsRectItem( mBorderItem );

	mPinsItem->setPen( QPen( Qt::black ) );
	mPinsItem->setBrush( mBackgroundColour );

	updateGui();

	if( NODE )
	{
		for( QSharedPointer<fugio::PinInterface> Pin : NODE->enumInputPins() )
		{
			pinInputAdd( Pin );
		}

		for( QSharedPointer<fugio::PinInterface> Pin : NODE->enumOutputPins() )
		{
			pinOutputAdd( Pin );
		}

		nodeStatusUpdated();
	}

	layoutPins();

	if( NODE )
	{
		setPos( NODE->setting( "editor/position", pPosition ).toPointF() );
	}

	updateToolTip();

	setFlags( flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges );
}

void NodeItem::layoutPins()
{
	const QFontMetrics		FM( mContextView->pinFont() );

	const qreal		InputWidth  = pinLabelWidth( mInputs );
	const qreal		OutputWidth = pinLabelWidth( mOutputs );
	const qreal		GuiWidth    = ( mNodeGui ? mNodeGui->size().width()  : 0 );
	const qreal		GuiHeight   = ( mNodeGui ? mNodeGui->size().height() : 0 );
	const qreal		LabelHeight = mLabelText->boundingRect().height();
	const qreal		MaxWidth    = qMax( InputWidth + 5 + GuiWidth + 5 + OutputWidth, mLabelText->boundingRect().width() + 2 + ( mStatusItem ? LabelHeight + 2 : 0 ) );

	prepareGeometryChange();

	mLabelItem->setRect( 0, 0, 5 + MaxWidth + 5, 5 + mLabelText->boundingRect().height() + 5 );

	if( mStatusItem )
	{
		mStatusItem->setRect( MaxWidth - LabelHeight + 5, 5, LabelHeight, LabelHeight );
	}

	mBorderItem->setRect( -1, -1, mLabelItem->rect().width() + 2, mLabelItem->rect().height() + 2 );

	int		PinPairIdx  = 0;
	int		PinTotalIdx = 0;
	PinPair	PP;
	int		i, PinIdx;

	for( i = 0, PinIdx = 0 ; i < mInputs.size() ; i++ )
	{
		PP = mInputs.at( i );

		// Paired Pin

		if( !PP.second->pin()->pairedUuid().isNull() )
		{
			continue;
		}

		// Trigger Pin

		if( !PP.first )
		{
			PP.second->setPos( -PP.second->boundingRect().width(), ( mLabelItem->rect().height() / 2.0 ) - ( PP.second->boundingRect().height() / 2.0 ) );

			continue;
		}

		PP.first->setPos( 5, mLabelItem->boundingRect().height() + ( FM.height() * PinIdx ) );

		PP.second->setPos( -PP.second->boundingRect().width(), PP.first->pos().y() + ( qreal( FM.height() ) / 2.0 ) - ( PP.second->boundingRect().height() / 2.0 ) );

		PinIdx++;

		PinPairIdx  = std::max( PinPairIdx,  PinIdx );
		PinTotalIdx = std::max( PinTotalIdx, PinIdx );
	}

	for( i = 0, PinIdx = 0 ; i < mOutputs.size() ; i++ )
	{
		PP = mOutputs.at( i );

		// Paired Pin

		if( !PP.second->pin()->pairedUuid().isNull() )
		{
			continue;
		}

		PP.first->setPos( 5 + MaxWidth - PP.first->boundingRect().width(), mLabelItem->boundingRect().height() + ( FM.height() * PinIdx ) );

		PP.second->setPos( 5 + MaxWidth + 5, PP.first->pos().y() + ( qreal( FM.height() ) / 2.0 ) - ( PP.second->boundingRect().height() / 2.0 ) );

		PinIdx++;

		PinPairIdx  = std::max( PinPairIdx,  PinIdx );
		PinTotalIdx = std::max( PinTotalIdx, PinIdx );
	}

	// Paired Pins

	for( i = 0, PinIdx = PinPairIdx ; i < mInputs.size() ; i++ )
	{
		PP = mInputs.at( i );

		// Paired Pin

		if( PP.second->pin()->pairedUuid().isNull() )
		{
			continue;
		}

		PP.first->setPos( 5, mLabelItem->boundingRect().height() + ( FM.height() * PinIdx ) );

		PP.second->setPos( -PP.second->boundingRect().width(), PP.first->pos().y() + ( qreal( FM.height() ) / 2.0 ) - ( PP.second->boundingRect().height() / 2.0 ) );

		PinIdx++;

		PinTotalIdx = std::max( PinTotalIdx, PinIdx );
	}

	for( i = 0, PinIdx = PinPairIdx ; i < mOutputs.size() ; i++ )
	{
		PP = mOutputs.at( i );

		// Paired Pin

		if( PP.second->pin()->pairedUuid().isNull() )
		{
			continue;
		}

		PP.first->setPos( 5 + MaxWidth - PP.first->boundingRect().width(), mLabelItem->boundingRect().height() + ( FM.height() * PinIdx ) );

		PP.second->setPos( 5 + MaxWidth + 5, PP.first->pos().y() + ( qreal( FM.height() ) / 2.0 ) - ( PP.second->boundingRect().height() / 2.0 ) );

		PinIdx++;

		PinTotalIdx = std::max( PinTotalIdx, PinIdx );
	}

	if( InputWidth > 0 || OutputWidth > 0 || GuiWidth > 0 )
	{
		mPinsItem->show();

		mPinsItem->setRect( 0, mLabelItem->boundingRect().height() - 1, 5 + MaxWidth + 5, 5 + qMax<qreal>( FM.height() * PinTotalIdx, GuiHeight + 5 ) );

		if( mNodeGui )
		{
			mNodeGui->setPos( 5 + InputWidth, mPinsItem->rect().top() + 5 );
		}

		mBorderItem->setRect( mBorderItem->rect().adjusted( 0, 0, 0, mPinsItem->rect().height() ) );
	}
	else
	{
		mPinsItem->hide();
	}
}

void NodeItem::updateGui()
{
	if( mNodeGui )
	{
		mNodeGui->deleteLater();

		mNodeGui = nullptr;
	}

	QSharedPointer<fugio::NodeInterface>			NODE = mContextView->context()->findNode( mNodeId );

	if( NODE )
	{
		if( NODE->status() == fugio::NodeInterface::Initialising )
		{
			QTimer::singleShot( 100, this, SLOT(updateGui()) );

			return;
		}

		if( NODE->control() )
		{
			QWidget		*NodeGui = NODE->control()->gui();

			if( NodeGui )
			{
				NodeGui->setObjectName( "NodeWidget" );

				if( ( mNodeGui = scene()->addWidget( NodeGui ) ) != 0 )
				{
					mNodeGui->setFlag( QGraphicsItem::ItemIgnoresTransformations );

					mNodeGui->setParentItem( this );
				}
				else
				{
					NodeGui->deleteLater();
				}
			}
		}

		mNodeName = NODE->name();
	}

	mLabelText->setText( mNodeName );

	updateToolTip();

	layoutPins();

	updateActiveState();
}

qreal NodeItem::pinLabelWidth( const QList<NodeItem::PinPair> &pPinPairs )
{
	qreal		MaxWidth = 0;

	for( int i = 0 ; i < pPinPairs.size() ; i++ )
	{
		PinPair		PP = pPinPairs.at( i );

		if( PP.first )
		{
			MaxWidth = qMax( MaxWidth, PP.first->boundingRect().width() );
		}
	}

	return( MaxWidth > 0.0 ? MaxWidth + 5.0 : 0.0 );
}

void NodeItem::updateActiveState()
{
	QSharedPointer<fugio::NodeInterface>			NODE = mContextView->context()->findNode( mNodeId );

	if( !NODE || NODE->isActive() )
	{
		mLabelItem->setBrush( mContextView->labelBrush() );
	}
	else
	{
		mLabelItem->setBrush( Qt::gray );
	}
}

void NodeItem::updateActiveState(const QBrush &)
{
	updateActiveState();
}

void NodeItem::updateLabelFont(const QFont &pFont)
{
	qDebug() << pFont;

	mLabelText->setFont( pFont );

	layoutPins();
}

void NodeItem::updatePinFont(const QFont &pFont)
{
	qDebug() << pFont;

	for( int i = 0 ; i < mInputs.size() ; i++ )
	{
		PinPair		PP = mInputs.at( i );

		PP.first->setFont( pFont );
	}

	for( int i = 0 ; i < mOutputs.size() ; i++ )
	{
		PinPair		PP = mOutputs.at( i );

		PP.first->setFont( pFont );
	}

	layoutPins();
}

void NodeItem::setColour( const QColor &pColor )
{
	QSharedPointer<fugio::NodeInterface>			NODE = mContextView->context()->findNode( mNodeId );

	if( NODE )
	{
		NODE->setSetting( "editor/colour", pColor.name() );
	}

	mBackgroundColour = pColor;

	mPinsItem->setBrush( mBackgroundColour );

	update();
}

void NodeItem::setNodePos( const QPointF &pPos )
{
	if( pos() != pPos )
	{
		setPos( pPos );
	}

	labelDrag( QPointF() );
}

void NodeItem::setNodePosSilent( const QPointF &pPos )
{
	if( pPos != pos() )
	{
		setFlags( flags() & ~QGraphicsItem::ItemSendsGeometryChanges );

		setNodePos( pPos );

		setFlags( flags() | QGraphicsItem::ItemSendsGeometryChanges );
	}
}

void NodeItem::setName(const QString &pName)
{
	mNodeName = pName;

	mLabelText->setText( mNodeName );

	layoutPins();

	pinLinkMove( mOutputs, QPointF() );
}

void NodeItem::moveStarted()
{
	mOffset = pos() - computeTopLeftGridPoint( pos() );

	mMoveStart = pos();

	mUndoId++;
}

NodeItem::PinPair *NodeItem::findPinPair(QSharedPointer<fugio::PinInterface> pPin)
{
	PinPair		*PP = nullptr;

	if( pPin->direction() == PIN_INPUT )
	{
		PP = findPinPairInput( pPin->globalId() );
	}
	else
	{
		PP = findPinPairOutput( pPin->globalId() );
	}

	return( PP );
}

bool NodeItem::comparePinPair( const PinPair &P1, const PinPair &P2 )
{
	return( P1.second->pin()->order() < P2.second->pin()->order() );
}

void NodeItem::pinNameChanged( QSharedPointer<fugio::PinInterface> pPin )
{
	if( PinPair *PP = findPinPair( pPin ) )
	{
		PP->first->setText( pPin->name() );

		layoutPins();
	}
}

void NodeItem::pinsPaired(QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2)
{
	Q_UNUSED( pPin1 )
	Q_UNUSED( pPin2 )

	layoutPins();
}

void NodeItem::pinsUnpaired(QSharedPointer<fugio::PinInterface> pPin1, QSharedPointer<fugio::PinInterface> pPin2)
{
	Q_UNUSED( pPin1 )
	Q_UNUSED( pPin2 )

	layoutPins();
}

void NodeItem::nodeStatusUpdated()
{
	if( QSharedPointer<fugio::NodeInterface> NODE = mContextView->context()->findNode( mNodeId ) )
	{
		switch( NODE->status() )
		{
			case fugio::NodeInterface::Initialising:
				mStatusItem->setBrush( Qt::gray );
				break;

			case fugio::NodeInterface::Deferred:
				mStatusItem->setBrush( Qt::blue );
				break;

			case fugio::NodeInterface::Warning:
				mStatusItem->setBrush( Qt::yellow );
				break;

			case fugio::NodeInterface::Initialised:
				mStatusItem->setBrush( Qt::green );
				break;

			case fugio::NodeInterface::Error:
				mStatusItem->setBrush( Qt::red );
				break;
		}

		mStatusItem->setToolTip( NODE->statusMessage() );
	}
}

void NodeItem::menuRename( void )
{
	QSharedPointer<fugio::NodeInterface>		NODE = mContextView->context()->findNode( mNodeId );
	bool								OK;

	QString		CurrName = NODE ? NODE->name() : mNodeName;

	QString		NextName = QInputDialog::getText( 0, "Name", "Name", QLineEdit::Normal, CurrName, &OK );

	if( OK && NextName != CurrName )
	{
		if( NODE )
		{
			if( CmdNodeRename *Cmd = new CmdNodeRename( NODE, NextName ) )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}
		else
		{
			if( CmdGroupRename *Cmd = new CmdGroupRename( mContextView, mNodeId, CurrName, NextName ) )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}
	}
}

void NodeItem::menuActive( bool pActive )
{
	if( CmdNodeSetActive *Cmd = new CmdNodeSetActive( mContextView->context()->findNode( mNodeId ), pActive ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void NodeItem::menuSetColour()
{
	QColor C = QColorDialog::getColor( mBackgroundColour );

	if( C != mBackgroundColour )
	{
		if( CmdNodeSetColour *Cmd = new CmdNodeSetColour( mContextView->findNodeItem( mNodeId ), C ) )
		{
			mContextView->widget()->undoStack()->push( Cmd );
		}
	}
}

void NodeItem::menuAddInputPin()
{
	QSharedPointer<fugio::NodeInterface>		NODE = mContextView->context()->findNode( mNodeId );

	QStringList		 PinLst = NODE->control()->availableInputPins();

	QString			 PIN_NAME;
	bool			 OK;

	if( !PinLst.isEmpty() )
	{
		PIN_NAME = QInputDialog::getItem( gApp->mainWindow(), "Name", "Name", PinLst, 0, !NODE->control()->mustChooseNamedInputPin(), &OK );
	}
	else
	{
		PIN_NAME = QInputDialog::getText( gApp->mainWindow(), "Name", "Name", QLineEdit::Normal, "", &OK );
	}

	if( OK )
	{
		QSharedPointer<fugio::PinInterface>	PIN = mContextView->context()->global()->createPin( PIN_NAME, QUuid::createUuid(), NODE, PIN_INPUT );

		if( PIN )
		{
			CmdNodePinAdd	*Cmd = new CmdNodePinAdd( NODE, PIN );

			if( Cmd )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}
	}
}

void NodeItem::menuAddOutputPin()
{
	QSharedPointer<fugio::NodeInterface>		NODE = mContextView->context()->findNode( mNodeId );

	QList<fugio::NodeControlInterface::AvailablePinEntry>		 PinLst = NODE->control()->availableOutputPins();
	QStringList													 PinNam;

	for( auto PinEnt : PinLst )
	{
		PinNam.append( PinEnt.mName );
	}

	QString		PIN_NAME;
	QUuid		PIN_UUID;
	bool		OK;

	if( !PinLst.isEmpty() )
	{
		PIN_NAME = QInputDialog::getItem( gApp->mainWindow(), "Name", "Name", PinNam, 0, !NODE->control()->mustChooseNamedOutputPin(), &OK );
	}
	else
	{
		PIN_NAME = QInputDialog::getText( gApp->mainWindow(), "Name", "Name", QLineEdit::Normal, "", &OK );
	}

	if( OK )
	{
		const QList<QUuid>	PinLstOut = NODE->control()->pinAddTypesOutput();

		if( PinLstOut.size() == 1 )
		{
			PIN_UUID = PinLstOut.first();
		}
		else
		{
			for( auto PinEnt : PinLst )
			{
				if( PinEnt.mName == PIN_NAME )
				{
					PIN_UUID = PinEnt.mType;

					break;
				}
			}

			if( PIN_UUID.isNull() )
			{
				QMap<QUuid,QString>		PinIds = gApp->global().pinIds();
				QStringList				PinTypes;

				for( const QUuid &id : PinLstOut )
				{
					PinTypes << PinIds.value( id );
				}

				qSort( PinTypes );

				QString			PinType = QInputDialog::getItem( gApp->mainWindow(), tr( "Pin Type" ), tr( "Choose Pin Type" ), PinTypes, 0, false, &OK );

				if( OK )
				{
					PIN_UUID = gApp->global().findPinByClass( PinType );
				}
			}
		}
	}

	if( OK )
	{
		QSharedPointer<fugio::PinInterface>	PIN = mContextView->context()->global()->createPin( PIN_NAME, QUuid::createUuid(), NODE, PIN_OUTPUT, PIN_UUID );

		if( PIN )
		{
			CmdNodePinAdd	*Cmd = new CmdNodePinAdd( NODE, PIN );

			if( Cmd )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}
	}

}

void NodeItem::menuDelete()
{
	if( CmdNodeRemove *Cmd = new CmdNodeRemove( mContextView->context(), mContextView->context()->findNode( mNodeId ) ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void NodeItem::menuHelp()
{
	QString		HelpUrl = helpUrl( mContextView->context()->findNode( mNodeId ) );

	if( !HelpUrl.isEmpty() )
	{
		QDesktopServices::openUrl( QUrl( HelpUrl.append( "?utm_source=fugio&utm_medium=node-help" ) ) );
	}
}

void NodeItem::menuUngroup()
{
	mContextView->ungroup( this );
}

PinItem *NodeItem::pinInputAdd( QSharedPointer<fugio::PinInterface> pPin )
{
	QGraphicsSimpleTextItem		*TextItem = nullptr;
	PinItem						*Pin = new PinItem( mContextView, pPin, this );

	if( pPin->localId() != PID_FUGIO_NODE_TRIGGER )
	{
		TextItem = new QGraphicsSimpleTextItem( pPin->name(), mPinsItem );

		TextItem->setFont( mContextView->pinFont() );
	}

	mInputs.append( PinPair( TextItem, Pin ) );

	std::sort( mInputs.begin(), mInputs.end(), &NodeItem::comparePinPair );

	if( pPin->localId() != PID_FUGIO_NODE_TRIGGER )
	{
		connect( pPin->qobject(), SIGNAL(nameChanged(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinNameChanged(QSharedPointer<fugio::PinInterface>)) );
	}

	return( Pin );
}

PinItem *NodeItem::pinOutputAdd( QSharedPointer<fugio::PinInterface> pPin )
{
	QGraphicsSimpleTextItem		*TextItem = new QGraphicsSimpleTextItem( pPin->name(), mPinsItem );
	PinItem						*Pin = new PinItem( mContextView, pPin, this );

	TextItem->setFont( mContextView->pinFont() );

	mOutputs.append( PinPair( TextItem, Pin ) );

	std::sort( mOutputs.begin(), mOutputs.end(), &NodeItem::comparePinPair );

	connect( pPin->qobject(), SIGNAL(nameChanged(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinNameChanged(QSharedPointer<fugio::PinInterface>)) );

	return( Pin );
}

void NodeItem::pinInputRemove( QSharedPointer<fugio::PinInterface> pPin )
{
	for( int i = 0 ; i < mInputs.size() ; i++ )
	{
		PinPair		PP = mInputs.at( i );

		if( PP.second->pin() == pPin )
		{
			mInputs.removeAt( i );

			delete PP.first;

			layoutPins();

			return;
		}
	}
}

void NodeItem::pinOutputRemove( QSharedPointer<fugio::PinInterface> pPin )
{
	for( int i = 0 ; i < mOutputs.size() ; i++ )
	{
		PinPair		PP = mOutputs.at( i );

		if( PP.second->pin() == pPin )
		{
			mOutputs.removeAt( i );

			delete PP.first;

			layoutPins();

			return;
		}
	}
}

PinItem *NodeItem::findPinInput( QSharedPointer<fugio::PinInterface> pPin )
{
	if( PinPair *PP = findPinPairInput( pPin->globalId() ) )
	{
		return( PP->second );
	}

	return( nullptr );
}

PinItem *NodeItem::findPinOutput( QSharedPointer<fugio::PinInterface> pPin )
{
	if( PinPair *PP = findPinPairOutput( pPin->globalId() ) )
	{
		return( PP->second );
	}

	return( nullptr );
}
