#include "statepin.h"
#include <QSettings>

#include <QOpenGLFunctions_2_0>

#define INSERT_FLAG(x)		mMapFlags.insert(#x,x)

QMap<QString,int>				 StatePin::mMapFlags;

StatePin::StatePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{
	INSERT_FLAG( GL_BLEND );
	INSERT_FLAG( GL_DEPTH_TEST );

#if !defined( GL_ES_VERSION_2_0 )
	INSERT_FLAG( GL_POINT_SPRITE );
	INSERT_FLAG( GL_POLYGON_SMOOTH );
	INSERT_FLAG( GL_PROGRAM_POINT_SIZE );
#endif

//	INSERT_FLAG( GL_PRIMITIVE_RESTART_FIXED_INDEX );

	mBlendRgbSrc = GL_SRC_ALPHA;
	mBlendAlphaSrc = GL_SRC_ALPHA;

	mBlendRgbDst = GL_ONE_MINUS_SRC_ALPHA;
	mBlendAlphaDst = GL_ONE_MINUS_SRC_ALPHA;

	mBlendRgbEquation = GL_FUNC_ADD;
	mBlendAlphaEquation = GL_FUNC_ADD;

	mDepthFunc = GL_LESS;

#if !defined( GL_ES_VERSION_2_0 )
	mPolygonMode = GL_FILL;
#endif
}

void StatePin::loadSettings( QSettings &pSettings )
{
	int		c = pSettings.beginReadArray( "flags" );

	for( int i = 0 ; i < c ; i++ )
	{
		pSettings.setArrayIndex( i );

		mFlags.append( pSettings.value( "flag" ).toInt() );
	}

	pSettings.endArray();

	mBlendRgbSrc = pSettings.value( "rgb-src", mBlendRgbSrc ).toInt();
	mBlendAlphaSrc = pSettings.value( "alpha-src", mBlendAlphaSrc ).toInt();

	mBlendRgbDst = pSettings.value( "rgb-dst", mBlendRgbDst ).toInt();
	mBlendAlphaDst = pSettings.value( "alpha-dst", mBlendAlphaDst ).toInt();

	mBlendRgbEquation = pSettings.value( "rgb-equation", mBlendRgbEquation ).toInt();
	mBlendAlphaEquation = pSettings.value( "alpha-equation", mBlendAlphaEquation ).toInt();

	mDepthFunc = pSettings.value( "depth-func", mDepthFunc ).toInt();

	mPolygonMode = pSettings.value( "polygon-mode", mPolygonMode ).toInt();
}

void StatePin::saveSettings( QSettings &pSettings ) const
{
	pSettings.beginWriteArray( "flags" );

	for( int i = 0 ; i < mFlags.size() ; i++ )
	{
		pSettings.setArrayIndex( i );

		pSettings.setValue( "flag", mFlags.at( i ) );
	}

	pSettings.endArray();

	pSettings.setValue( "rgb-src", mBlendRgbSrc );
	pSettings.setValue( "alpha-src", mBlendAlphaSrc );

	pSettings.setValue( "rgb-dst", mBlendRgbDst );
	pSettings.setValue( "alpha-dst", mBlendAlphaDst );

	pSettings.setValue( "rgb-equation", mBlendRgbEquation );
	pSettings.setValue( "alpha-equation", mBlendAlphaEquation );

	pSettings.setValue( "depth-func", mDepthFunc );

	pSettings.setValue( "polygon-mode", mPolygonMode );
}

void StatePin::stateBegin()
{
	initializeOpenGLFunctions();

	QOpenGLFunctions_2_0	*GL20 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();

	if( GL20 && !GL20->initializeOpenGLFunctions() )
	{
		GL20 = Q_NULLPTR;
	}

#if !defined( GL_ES_VERSION_2_0 )
//	QMatrix4x4		MatPrj = mPinMatPrj ? variant( mPinMatPrj ).value<QMatrix4x4>() : mProjection;
//	QMatrix4x4		MatMod = mPinMatMod ? variant( mPinMatMod ).value<QMatrix4x4>() : mModelView;

//	glMatrixMode( GL_PROJECTION );
//	glLoadMatrixf( MatPrj.data() );

//	glMatrixMode( GL_MODELVIEW );
//	glLoadMatrixf( MatMod.data() );
#endif

	for( QList<int>::const_iterator it = mFlags.begin() ; it != mFlags.end() ; it++ )
	{
		glEnable( *it );
	}

	if( mFlags.contains( GL_BLEND ) )
	{
		glBlendFuncSeparate( mBlendRgbSrc, mBlendRgbDst, mBlendAlphaSrc, mBlendAlphaDst );

		glBlendEquationSeparate( mBlendRgbEquation, mBlendAlphaEquation );
	}

	if( mFlags.contains( GL_DEPTH_TEST ) )
	{
		glDepthFunc( mDepthFunc );
	}

//	glColor4f( 1, 1, 1, 1 );

	if( GL20 )
	{
		GL20->glPolygonMode( GL_FRONT_AND_BACK, mPolygonMode );
	}
}

void StatePin::stateEnd()
{
	initializeOpenGLFunctions();

	QOpenGLFunctions_2_0	*GL20 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();

	if( GL20 && !GL20->initializeOpenGLFunctions() )
	{
		GL20 = Q_NULLPTR;
	}

	for( QList<int>::const_iterator it = mFlags.begin() ; it != mFlags.end() ; it++ )
	{
		glDisable( *it );
	}

	if( mFlags.contains( GL_BLEND ) )
	{
		glBlendFunc( GL_ONE, GL_ZERO );

		glBlendEquation( GL_FUNC_ADD );
	}

	if( mFlags.contains( GL_DEPTH_TEST ) )
	{
		glDepthFunc( GL_LESS );
	}

//	glColor4f( 1, 1, 1, 1 );

	if( GL20 )
	{
		GL20->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
}

void StatePin::setFlag( int pFlag, bool pValue )
{
	mFlags.removeAll( pFlag );

	if( pValue )
	{
		mFlags.append( pFlag );
	}
}

bool StatePin::tstFlag( int pFlag ) const
{
	return( mFlags.contains( pFlag ) );
}
