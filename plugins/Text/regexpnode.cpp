#include "regexpnode.h"

#include <QLineEdit>
#include <QDebug>

#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>

RegExpNode::RegExpNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	mPinInput = pinInput( "Input" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_STRING );

	mRegExp = QRegExp( "(\\d+)" );
}

RegExpNode::~RegExpNode()
{

}

QWidget *RegExpNode::gui()
{
	QLineEdit		*GUI = new QLineEdit( mRegExp.pattern() );

	connect( GUI, SIGNAL(textEdited(QString)), this, SLOT(patternUpdate(QString)) );

	return( GUI );
}

void RegExpNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QString			Text = variant( mPinInput ).toString();

	if( Text.isEmpty() )
	{
		return;
	}

	QStringList			TextOut;

	for( int pos = mRegExp.indexIn( Text, 0 ) ; pos != -1 ; pos = mRegExp.indexIn( Text, pos + mRegExp.matchedLength() ) )
	{
		TextOut << mRegExp.cap( 1 );
	}

	if( TextOut != mValOutput->variant().toStringList() )
	{
		mValOutput->setVariant( TextOut );

		pinUpdated( mPinOutput );
	}
}

void RegExpNode::patternUpdate( const QString &pPattern )
{
	mRegExp.setPattern( pPattern );

	mNode->context()->updateNode( mNode );
}
