#include "viewportmatrixnode.h"

#include <QSurface>

#include <fugio/core/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/math/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/opengl/state_interface.h>
#include <fugio/pin_variant_iterator.h>
#include <fugio/core/list_interface.h>

#include "openglplugin.h"

ViewportMatrixNode::ViewportMatrixNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mUpdateViewport( true )
{
	FUGID( PIN_INPUT_RENDER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_X, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_INPUT_Y, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_INPUT_PROJECTION, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_INPUT_VIEW, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
	FUGID( PIN_OUTPUT_RENDER, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
	FUGID( PIN_OUTPUT_PROJECTION, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );
	FUGID( PIN_OUTPUT_VIEW, "51297977-7b4b-4e08-9dea-89a8add4abe0" );
	FUGID( PIN_OUTPUT_INDEX, "c997473a-2016-466b-9128-beacb99870a2" );

	mPinInputRender = pinInput( "Render", PIN_INPUT_RENDER );

	mPinX = pinInput( "X", PIN_INPUT_X );
	mPinY = pinInput( "Y", PIN_INPUT_Y );

	mPinInputProjections = pinInput( "Projection", PIN_INPUT_PROJECTION );
	mPinInputViews       = pinInput( "View", PIN_INPUT_VIEW );

	mPinX->setValue( 1 );
	mPinY->setValue( 1 );

	mValOutput = pinOutput<fugio::RenderInterface *>( "Render", mPinOutput, PID_RENDER, PIN_OUTPUT_RENDER );

	mValOutputProjection = pinOutput<fugio::VariantInterface *>( "Projection", mPinOutputProjection, PID_MATRIX4, PIN_OUTPUT_PROJECTION );

	mValOutputView = pinOutput<fugio::VariantInterface *>( "View", mPinOutputView, PID_MATRIX4, PIN_OUTPUT_VIEW );

	mValOutputIndex = pinOutput<fugio::VariantInterface *>( "Index", mPinOutputIndex, PID_INTEGER, PIN_OUTPUT_INDEX );
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

	fugio::PinVariantIterator	Projection( mPinInputProjections );
	fugio::PinVariantIterator	View( mPinInputViews );

	QVector<int>	PinCnt = { Projection.count(), View.count() };

	auto			MinMax = std::minmax_element( PinCnt.begin(), PinCnt.end() );

	if( !*MinMax.first )
	{
		return;
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

			if( !Projection.isEmpty() )
			{
				MatPrj = Projection.index( MatIdx ).value<QMatrix4x4>();
			}

			if( !View.isEmpty() )
			{
				MatCam = View.index( MatIdx ).value<QMatrix4x4>();
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
