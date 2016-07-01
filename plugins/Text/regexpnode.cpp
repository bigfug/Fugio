#include "regexpnode.h"

#include <QLineEdit>
#include <QDebug>

#include <fugio/pin_control_interface.h>
#include <fugio/context_interface.h>

RegExpNode::RegExpNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_TEXT,	"AF7205D3-204D-4070-AAD0-3EF106935733" );
	FUGID( PIN_OUTPUT_TEXT,	"ABDF2527-5590-4FDC-AEC7-E7F93E7E7E89" );

	mPinInput = pinInput( "Input", PIN_INPUT_TEXT);

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_STRING_LIST, PIN_OUTPUT_TEXT );

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
