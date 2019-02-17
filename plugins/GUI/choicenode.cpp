#include "choicenode.h"

#include <QInputDialog>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>

#include <fugio/core/uuid.h>
#include <fugio/choice_interface.h>
#include <fugio/pin_signals.h>
#include <fugio/node_signals.h>

#include <QSettings>

#include <limits>

ChoiceNode::ChoiceNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mValChoice = pinOutput<fugio::VariantInterface *>( "Choice", mPinChoice, PID_STRING );
}

bool ChoiceNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	return( true );
}

QWidget *ChoiceNode::gui()
{
	QPushButton		*GUI = new QPushButton();

	if( GUI )
	{
		GUI->setEnabled( false );

		connect( GUI, SIGNAL(released()), this, SLOT(clicked()) );

		connect( this, &ChoiceNode::guiTextUpdated, GUI, &QPushButton::setText );

		connect( this, &ChoiceNode::guiEnabled, GUI, &QPushButton::setEnabled );
	}

	if( mPinChoice->isConnected() )
	{
		pinLinked( mPinChoice->connectedPin() );
	}

	connect( mPinChoice->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinLinked(QSharedPointer<fugio::PinInterface>)) );

	connect( mPinChoice->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(pinUnlinked(QSharedPointer<fugio::PinInterface>)) );

	return( GUI );
}

void ChoiceNode::loadSettings( QSettings &pSettings )
{
	NodeControlBase::loadSettings( pSettings );

	valueChanged( pSettings.value( "value" ).toString() );
}

void ChoiceNode::saveSettings( QSettings &pSettings ) const
{
	NodeControlBase::saveSettings( pSettings );

	pSettings.setValue( "value", mValChoice->variant().toString() );
}

void ChoiceNode::valueChanged( const QString &pValue )
{
	if( pValue.isEmpty() || pValue == mValChoice->variant().toString() )
	{
		return;
	}

	mValChoice->setVariant( pValue );

	pinUpdated( mPinChoice );

	emit valueUpdated( pValue );
}

void ChoiceNode::pinLinked( QSharedPointer<fugio::PinInterface> P )
{
	fugio::ChoiceInterface	*CI = P->hasControl() ? qobject_cast<fugio::ChoiceInterface *>( P->control()->qobject() ) : nullptr;

	if( CI )
	{
		QStringList		Choices = CI->choices();
		QString			Choice  = mValChoice->variant().toString().isEmpty() ? P->value().toString() : mValChoice->variant().toString();

		if( !Choices.isEmpty() && ( Choice.isEmpty() || !Choices.contains( Choice ) ) )
		{
			Choice = Choices.first();
		}

		emit guiTextUpdated( Choice );

		emit guiEnabled( true );

		valueChanged( Choice );

		connect( dynamic_cast<QObject *>( CI ), SIGNAL(choicesChanged()), this, SLOT(choicesChanged()) );
	}
	else
	{
		emit guiTextUpdated( QString() );

		emit guiEnabled( false );
	}
}

void ChoiceNode::pinUnlinked( QSharedPointer<fugio::PinInterface> P )
{
	fugio::ChoiceInterface	*CI = P->hasControl() ? qobject_cast<fugio::ChoiceInterface *>( P->control()->qobject() ) : nullptr;

	if( CI )
	{
		disconnect( dynamic_cast<QObject *>( CI ), SIGNAL(choicesChanged()), this, SLOT(choicesChanged()) );
	}

	emit guiTextUpdated( QString() );

	emit guiEnabled( false );
}

void ChoiceNode::clicked()
{
	QSharedPointer<fugio::PinInterface> P = mPinChoice->connectedPin();

	if( !P->hasControl() )
	{
		return;
	}

	fugio::ChoiceInterface	*CI = qobject_cast<fugio::ChoiceInterface *>( P->control()->qobject() );

	if( CI )
	{
		QStringList		Choices = CI->choices();
		QString			Choice  = mValChoice->variant().toString().isEmpty() ? P->value().toString() : mValChoice->variant().toString();

		if( !Choices.isEmpty() && ( Choice.isEmpty() || !Choices.contains( Choice ) ) )
		{
			Choice = Choices.first();
		}

		QString	NewChoice = QInputDialog::getItem( nullptr, tr( "Select" ), QString(), Choices, Choices.indexOf( Choice ), false );

		if( !NewChoice.isEmpty() && NewChoice != Choice )
		{
			valueChanged( NewChoice );
		}
	}
}

void ChoiceNode::choicesChanged()
{

}
