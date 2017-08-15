#include "vertexarrayobjectpin.h"

VertexArrayObjectPin::VertexArrayObjectPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

QString VertexArrayObjectPin::toString() const
{
	return( QString() );
}

QString VertexArrayObjectPin::description() const
{
	return( QString() );
}
