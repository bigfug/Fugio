#include "parampin.h"

ParamPin::ParamPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

#if defined( ARTOOLKIT_SUPPORTED )

const ARParam &ParamPin::param() const
{
	return( mParam );
}

void ParamPin::setParam( const ARParam &pParam )
{
	mParam = pParam;
}

#endif
