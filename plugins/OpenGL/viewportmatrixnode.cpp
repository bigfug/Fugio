#include "viewportmatrixnode.h"

#include <QSurface>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/math/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/state_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/list_interface.h>

#include "openglplugin.h"

ViewportMatrixNode::ViewportMatrixNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mUpdateViewport( true )
{
	mPinInputRender = pinInput( "Render" );

	mPinX = pinInput( "X" );
	mPinY = pinInput( "Y" );

	mPinInputProjections = pinInput( "Projections" );
	mPinInputViews       = pinInput( "Views" );

	mPinX->setValue( 1 );
	mPinY->setValue( 1 );

	mValOutput = pinOutput<fugio::RenderInterface *>( "Render", mPinOutput, PID_RENDER );

	mValOutputProjection = pinOutput<fugio::VariantInterface *>( "Projection", mPinOutputProjection, PID_MATRIX4 );

	mValOutputView = pinOutput<fugio::VariantInterface *>( "View", mPinOutputView, PID_MATRIX4 );

	mValOutputIndex = pinOutput<fugio::VariantInterface *>( "Index", mPinOutputIndex, PID_INTEGER );
}

void ViewportMatrixNode::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	mUpdateViewport = true;

	pinUpdated( mPinOutput );
}

QList<QUuid> ViewportMatrixNode::pinAddTypesInput() const
{
	QList<QUuid>		PinIds;

	PinIds << PID_OPENGL_STATE;
	PinIds << PID_RENDER;

	return( PinIds );
}

bool ViewportMatrixNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void ViewportMatrixNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	fugio::RenderInterface		*Render = input<fugio::RenderInterface *>( mPinInputRender );

	if( !Render )
	{
		return;
	}

	fugio::ArrayInterface		*PrjArr = input<fugio::ArrayInterface *>( mPinInputProjections );
	fugio::ArrayInterface		*CamArr = input<fugio::ArrayInterface *>( mPinInputViews );

	if( PrjArr && PrjArr->type() != QMetaType::QMatrix4x4 )
	{
		PrjArr = 0;
	}

	if( CamArr && CamArr->type() != QMetaType::QMatrix4x4 )
	{
		CamArr = 0;
	}

	fugio::ListInterface		*PrjLst = input<fugio::ListInterface *>( mPinInputProjections );
	fugio::ListInterface		*CamLst = input<fugio::ListInterface *>( mPinInputViews );

	if( PrjLst && PrjLst->listPinControl() != PID_MATRIX4 )
	{
		PrjLst = 0;
	}

	if( CamLst && CamLst->listPinControl() != PID_MATRIX4 )
	{
		CamLst = 0;
	}

	GLint			TmpVP[ 4 ];

	glGetIntegerv( GL_VIEWPORT, TmpVP );

	const QRect		SrcVP( TmpVP[ 0 ], TmpVP[ 1 ], TmpVP[ 2 ], TmpVP[ 3 ] );

	const int		CntX = qMax( 1, variant( mPinX ).toInt() );
	const int		CntY = qMax( 1, variant( mPinY ).toInt() );
	const int		SzeX = SrcVP.width()  / CntX;
	const int		SzeY = SrcVP.height() / CntY;

	QMatrix4x4		MatPrj;
	QMatrix4x4		MatCam;

	for( int y = 0 ; y < CntY ; y++ )
	{
		for( int x = 0 ; x < CntX ; x++ )
		{
			const int			MatIdx = ( y * CntX ) + x;

			if( PrjArr && PrjArr->count() > MatIdx )
			{
				MatPrj = QMatrix4x4( &reinterpret_cast<const float *>( PrjArr->array() )[ MatIdx * 16 ] );
			}

			if( CamArr && CamArr->count() > MatIdx )
			{
				MatCam = QMatrix4x4( &reinterpret_cast<const float *>( CamArr->array() )[ MatIdx * 16 ] );
			}

			mValOutputProjection->setVariant( MatPrj );
			mValOutputView->setVariant( MatCam );
			mValOutputIndex->setVariant( MatIdx );

			const QRect			DstVP( SrcVP.x() + x * SzeX, SrcVP.y() + y * SzeY, SzeX, SzeY );

			glViewport( DstVP.x(), DstVP.y(), DstVP.width(), DstVP.height() );

			Render->render( pTimeStamp );
		}
	}

	glViewport( SrcVP.x(), SrcVP.y(), SrcVP.width(), SrcVP.height() );
}
