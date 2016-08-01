#include "stringnode.h"

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>

#include <fugio/core/uuid.h>

#include <QLineEdit>
#include <QDateTime>
#include <QSettings>

#include <limits>

StringNode::StringNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastUpdate( 0 ), mLastText( 0 )
{
	FUGID( PIN_INPUT_BUFFER,	"1AE07069-DB1A-4E50-9294-4C725F9CDFA2" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInput = pinInput( "Buffer", PIN_INPUT_BUFFER );

	mPinInput->registerPinInputType( PID_BOOL );

	mString = pinOutput<fugio::VariantInterface *>( "String", mPinValue, PID_STRING );
}

StringNode::~StringNode( void )
{
}

QWidget *StringNode::gui()
{
	QLineEdit	*GUI = new QLineEdit();

	GUI->setText( mText );

	connect( GUI, SIGNAL(textChanged(QString)), this, SLOT(valueChanged(QString)) );

	connect( GUI, SIGNAL(editingFinished()), this, SLOT(editingFinished()) );

	connect( this, SIGNAL(valueUpdated(QString)), GUI, SLOT(setText(QString)) );

	return( GUI );
}

void StringNode::loadSettings( QSettings &pSettings )
{
	valueChanged( pSettings.value( "Value" ).toString() );
}

void StringNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "Value", mString->variant().toString() );
}

void StringNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	pinUpdated( mPinValue );
}

void StringNode::valueChanged( const QString &pValue )
{
	if( pValue == mText )
	{
		return;
	}

	mText = pValue;

	mLastText = QDateTime::currentMSecsSinceEpoch();

	if( !variant( mPinInput ).toBool() )
	{
		connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
	}

	emit valueUpdated( pValue );
}

void StringNode::editingFinished()
{
	QLineEdit	*GUI = qobject_cast<QLineEdit *>( sender() );

	if( !GUI )
	{
		return;
	}

	valueChanged( GUI->text() );

	if( variant( mPinInput ).toBool() )
	{
		connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
	}
}

void StringNode::contextFrameStart()
{
	if( mLastUpdate < mLastText )
	{
		mString->setVariant( QString( mText.toLatin1().constData() ) );

		pinUpdated( mPinValue );

		mLastUpdate = mLastText;
	}

	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(contextFrameStart()) );
}
