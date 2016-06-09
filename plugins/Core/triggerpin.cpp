#include "triggerpin.h"

TriggerPin::TriggerPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

TriggerPin::~TriggerPin()
{

}
