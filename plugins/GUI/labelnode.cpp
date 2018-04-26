#include "labelnode.h"

#include <QBrush>

LabelNode::LabelNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInput = pinInput( "", PIN_INPUT_STRING );
}

//QWidget *LabelNode::gui()
//{
//	QLabel		*GUI = new QLabel();

//	GUI->setText( mValInput );

//	connect( this, SIGNAL(valueUpdated(QString)), GUI, SLOT(setText(QString)) );

//	return( GUI );
//}

void LabelNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QString		NewVal = variant<QString>( mPinInput );

	if( NewVal != mValInput )
	{
		mValInput = NewVal;

		if( mTextItem )
		{
			mTextItem->setHtml( QString("<div style='background:rgba(255, 255, 255, 100%);padding: 1em;margin: 1em;'>" + NewVal + QString("</div>") ) );
		}
	}
}

QGraphicsItem *LabelNode::guiItem( void )
{
	QGraphicsTextItem	*GUI = mTextItem;

	if( !GUI )
	{
		GUI = new QGraphicsTextItem();

		mTextItem = GUI;

		mTextItem->setHtml( QString("<div style='background:rgba(255, 255, 255, 100%);padding: 1em;margin: 1em;'>" + mValInput + QString("</div>") ) );
	}

	return( GUI );
}
