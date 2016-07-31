#include "floatnode.h"
#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/core/uuid.h>

#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QDateTime>
#include <QSettings>

#include <limits>

FloatNode::FloatNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mFloat( 0 )
{
	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mFloat = pinOutput<fugio::VariantInterface *>( "Number", mPinValue, PID_FLOAT );

	mPinValue->setDescription( tr( "The output Number" ) );
}

FloatNode::~FloatNode( void )
{
}

QWidget *FloatNode::gui()
{
	QLineEdit		*GUI = new QLineEdit();

	GUI->setText( QString::number( mFloat->variant().toDouble() ) );

	connect( GUI, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)) );

	connect( this, SIGNAL(textUpdated(QString)), GUI, SLOT(setText(QString)) );

	return( GUI );
}

void FloatNode::loadSettings( QSettings &pSettings )
{
	mFloat->setVariant( pSettings.value( "Value" ).toDouble() );

	emit textUpdated( QString::number( mFloat->variant().toDouble() ) );

	pinUpdated( mPinValue );
}

void FloatNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "Value", mFloat->variant().toDouble() );
}

void FloatNode::textChanged( const QString &pText )
{
	bool		b;
	double		v = pText.toDouble( &b );

	if( !b || v == mFloat->variant().toDouble() )
	{
		return;
	}

	mFloat->setVariant( v );

	pinUpdated( mPinValue );
}


void FloatNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinValue );
}
