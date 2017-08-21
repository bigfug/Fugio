#include "vertexarrayobjectpin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

VertexArrayObjectPin::VertexArrayObjectPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

QString VertexArrayObjectPin::toString() const
{
	return( QString::number( vaoId() ) );
}

QString VertexArrayObjectPin::description() const
{
	return( QString() );
}

fugio::VertexArrayObjectInterface *VertexArrayObjectPin::vaoParent()
{
	if( !mPin->node()->hasControl() )
	{
		return( Q_NULLPTR );
	}

	return( qobject_cast<fugio::VertexArrayObjectInterface *>( mPin->node()->control()->qobject() ) );
}

fugio::VertexArrayObjectInterface *VertexArrayObjectPin::vaoParent() const
{
	if( !mPin->node()->hasControl() )
	{
		return( Q_NULLPTR );
	}

	return( qobject_cast<fugio::VertexArrayObjectInterface *>( mPin->node()->control()->qobject() ) );
}

void VertexArrayObjectPin::vaoBind()
{
	fugio::VertexArrayObjectInterface	*IG = vaoParent();

	if( IG )
	{
		IG->vaoBind();
	}
}

void VertexArrayObjectPin::vaoRelease()
{
	fugio::VertexArrayObjectInterface	*IG = vaoParent();

	if( IG )
	{
		IG->vaoRelease();
	}
}

GLuint VertexArrayObjectPin::vaoId() const
{
	fugio::VertexArrayObjectInterface	*IG = vaoParent();

	return( IG ? IG->vaoId() : -1 );
}
