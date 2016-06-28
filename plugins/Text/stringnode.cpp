#include "stringnode.h"

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/core/uuid.h>

#include <QLineEdit>
#include <QDateTime>
#include <QSettings>

#include <limits>

StringNode::StringNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mString = pinOutput<fugio::VariantInterface *>( "String", mPinValue, PID_STRING );
}

StringNode::~StringNode( void )
{
}

QWidget *StringNode::gui()
{
	QLineEdit	*GUI = new QLineEdit();

	GUI->setText( mString->variant().toString() );

	connect( GUI, SIGNAL(textChanged(QString)), this, SLOT(valueChanged(QString)) );

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

void StringNode::valueChanged( const QString &pValue )
{
	if( pValue == mString->variant().toString() )
	{
		return;
	}

	//qDebug() << pValue.toLatin1();

	mString->setVariant( QString( pValue.toLatin1().constData() ) );

	pinUpdated( mPinValue );

	emit valueUpdated( pValue );
}
