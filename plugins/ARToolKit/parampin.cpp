#include "parampin.h"

ParamPin::ParamPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

const ARParam &ParamPin::param() const
{
	return( mParam );
}

void ParamPin::setParam( const ARParam &pParam )
{
	mParam = pParam;
}
