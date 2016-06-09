#include "fftpin.h"

FFTPin::FFTPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mFFT( 0 )
{
}
