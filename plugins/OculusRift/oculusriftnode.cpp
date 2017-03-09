#include "oculusriftnode.h"

#include <QMatrix4x4>

#include <fugio/math/uuid.h>
#include <fugio/opengl/uuid.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/state_interface.h>
#include <fugio/performance.h>
#include <fugio/context_signals.h>

#include "oculusriftplugin.h"

OculusRiftNode::OculusRiftNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mOculusRift( 0 )
{
	static const QUuid		PIN_VIEW_MATRIX( "{bd6540d0-20f7-46de-b6bd-f56685b5390b}" );
	static const QUuid		PIN_STATE( "{49c4f1e9-4a4b-413f-a1c0-c10ab28c7884}" );
	static const QUuid		PIN_NEAR_PLANE( "{cafba06d-d7cb-416f-a093-7a52bc066582}" );
	static const QUuid		PIN_FAR_PLANE( "{8b9c6ec2-5613-466e-a297-8318a528cba4}" );

	mPinViewMatrix = pinInput( "View", PIN_VIEW_MATRIX );

	QSharedPointer<fugio::PinInterface>	PinState    = pinInput( "State", PIN_STATE );
	QSharedPointer<fugio::PinInterface>	PinGeometry = pinInput( "Geometry" );

	mPinNearPlane = pinInput( "Near Plane", PIN_NEAR_PLANE );
	mPinFarPlane  = pinInput( "Far Plane", PIN_FAR_PLANE );

	mPinNearPlane->setValue( 0.1 );
	mPinFarPlane->setValue( 1000.0 );

	mGeometry = pinOutput<fugio::RenderInterface *>( "Render", mPinGeometry, PID_RENDER );

	mProjection = pinOutput<fugio::VariantInterface *>( "Projection", mPinProjection, PID_MATRIX4 );

	mView = pinOutput<fugio::VariantInterface *>( "View", mPinView, PID_MATRIX4 );

	mPinViewMatrix->setDescription( tr( "The input matrix that describes the current position and orientation of the user's head in world space" ) );

	PinState->setDescription( tr( "The OpenGL rendering state to apply" ) );

	mPinNearPlane->setDescription( tr( "The near plane" ) );
	mPinFarPlane->setDescription( tr( "The far plane" ) );

	PinGeometry->setDescription( tr( "The input 3D Geometry to render" ) );

	mPinGeometry->setDescription( tr( "The output geometry - connect to an OpenGL Window" ) );

	mPinProjection->setDescription( tr( "The output matrix that describes the current frustum of the user's left/right eye" ) );

	mPinView->setDescription( tr( "The output matrix that describes the current position and orientation of the user's left/right eye in world space"));
}

bool OculusRiftNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OculusRiftPlugin::instance()->hasOpenGLContext() )
	{
		return( false );
	}

#if defined( OCULUS_PLUGIN_SUPPORTED )
	if( !mOculusRift && ( mOculusRift = DeviceOculusRift::newDevice() ) == Q_NULLPTR )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onContextFrame()) );
#endif

	mNode->context()->nodeInitialised();

	return( true );
}

bool OculusRiftNode::deinitialise()
{
#if defined( OCULUS_PLUGIN_SUPPORTED )
	disconnect( mNode->context()->qobject(), SIGNAL(frameStart()), this, SLOT(onContextFrame()) );

	mOculusRift.clear();
#endif

	return( NodeControlBase::deinitialise() );
}

void OculusRiftNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	pinUpdated( mPinGeometry );
}

QList<QUuid> OculusRiftNode::pinAddTypesInput() const
{
	QList<QUuid>	PinLst;

	PinLst << PID_OPENGL_STATE;
	PinLst << PID_RENDER;

	return( PinLst );
}

bool OculusRiftNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() == PIN_OUTPUT )
	{
		return( true );
	}

	return( false );
}

void OculusRiftNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	Q_UNUSED( pSourcePinId )

#if !defined( OCULUS_PLUGIN_SUPPORTED )
	Q_UNUSED( pTimeStamp )
#else
	fugio::Performance	Perf( mNode, "drawGeometry", pTimeStamp );

	if( !mNode->isInitialised() )
	{
		return;
	}

	if( !mOculusRift->hmd() )
	{
		return;
	}

	// We need to keep a reference to ourselves here as ovr_SubmitFrame can
	// call the main app event loop, which can close the context and delete us!

	QSharedPointer<fugio::NodeControlInterface>	C = mNode->control();

	mOculusRift->drawStart();

	const float		NearPlane = variant( mPinNearPlane ).toFloat();
	const float		FarPlane  = variant( mPinFarPlane ).toFloat();

	//	float Yaw(3.141592f);
	//	Vector3f Pos2(0.0f,1.6f,-5.0f);
	//	Pos2.y = ovrHmd_GetFloat(mHMD, OVR_KEY_EYE_HEIGHT, Pos2.y);

	QMatrix4x4		MatEye = variant( mPinViewMatrix ).value<QMatrix4x4>();
	QVector3D		TrnEye = MatEye.column( 3 ).toVector3D();

	MatEye.setColumn( 3, QVector4D( 0, 0, 0, 1 ) );

	const Vector3f Pos2( TrnEye.x(), TrnEye.y(), TrnEye.z() );

	Matrix4f tempRollPitchYaw;

	memcpy( tempRollPitchYaw.M, MatEye.constData(), sizeof( float ) * 16 );

	const Matrix4f rollPitchYaw = tempRollPitchYaw;

	// Render Scene to Eye Buffers
	for (int eye = 0; eye < 2; eye++)
	{
		mOculusRift->drawEyeStart( eye );

		// Get view and projection matrices
		//Matrix4f rollPitchYaw = Matrix4f( MatEye.transposed().data() );
		Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f( mOculusRift->eyeRenderPos( eye ).Orientation);
		Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
		Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
		Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform( mOculusRift->eyeRenderPos( eye ).Position );

		Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
		Matrix4f proj = ovrMatrix4f_Projection( mOculusRift->defaultEyeFOV( eye ), NearPlane, FarPlane, ovrProjection_None );

		mProjection->setVariant( QMatrix4x4( &proj.M[ 0 ][ 0 ], 4, 4 ).transposed() );

		mView->setVariant( QMatrix4x4( &view.M[ 0 ][ 0 ], 4, 4 ).transposed() );

		fugio::OpenGLStateInterface		*CurrentState = 0;

		for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
		{
			if( !P->isConnected() )
			{
				continue;
			}

			if( P->connectedPin().isNull() )
			{
				continue;
			}

			if( P->connectedPin()->control().isNull() )
			{
				continue;
			}

			QObject					*O = P->connectedPin()->control()->qobject();

			if( !O )
			{
				continue;
			}

			if( true )
			{
				fugio::RenderInterface		*Geometry = qobject_cast<fugio::RenderInterface *>( O );

				if( Geometry )
				{
					Geometry->render( pTimeStamp );

					continue;
				}
			}

			if( true )
			{
				fugio::OpenGLStateInterface		*NextState = qobject_cast<fugio::OpenGLStateInterface *>( O );

				if( NextState != 0 )
				{
					if( CurrentState != 0 )
					{
						CurrentState->stateEnd();
					}

					CurrentState = NextState;

					CurrentState->stateBegin();

					continue;
				}
			}
		}

		if( CurrentState != 0 )
		{
			CurrentState->stateEnd();
		}

		mOculusRift->drawEyeEnd( eye );
	}

	mOculusRift->drawEnd();

	pinUpdated( mPinProjection );
	pinUpdated( mPinView );
#endif
}

void OculusRiftNode::onContextFrame()
{
	pinUpdated( mPinGeometry );
}
