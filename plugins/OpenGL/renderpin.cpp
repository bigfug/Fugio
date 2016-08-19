
#include "renderpin.h"

#include <QSettings>

#if defined( Q_OS_WIN )
#include <gl/GLU.h>
#elif defined( Q_OS_MAC )
#include <glu.h>
#endif

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/opengl/node_render_interface.h>

RenderPin::RenderPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
}

RenderPin::~RenderPin()
{
}

void RenderPin::render( qint64 pTimeStamp )
{
	if( !mPin->node()->hasControl() )
	{
		return;
	}

	fugio::NodeRenderInterface	*IG = qobject_cast<fugio::NodeRenderInterface *>( mPin->node()->control()->qobject() );

	if( IG )
	{
		IG->render( pTimeStamp, mPin->localId() );
	}
}
