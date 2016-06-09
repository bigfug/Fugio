#include "fugio.h"
#include "globalprivate.h"

FUGIOLIBSHARED_EXPORT fugio::GlobalInterface *fugio::fugio( void )
{
	return( GlobalPrivate::instance() );
}
