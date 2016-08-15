#include "shaderpin.h"

ShaderPin::ShaderPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mShader( 0 )
{

}
