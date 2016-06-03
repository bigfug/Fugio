#include "choicepin.h"

ChoicePin::ChoicePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

QStringList ChoicePin::choices() const
{
	return( mChoices );
}

void ChoicePin::setChoices(QStringList pChoices)
{
	mChoices = pChoices;
}
