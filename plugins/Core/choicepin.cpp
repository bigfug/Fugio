#include "choicepin.h"

ChoicePin::ChoicePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

QString ChoicePin::toString() const
{
	return( mPin->value().toString() );
}

QStringList ChoicePin::choices() const
{
	return( mChoices );
}

void ChoicePin::setChoices(QStringList pChoices)
{
	mChoices = pChoices;
}
