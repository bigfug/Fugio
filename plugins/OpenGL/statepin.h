#ifndef STATEPIN_H
#define STATEPIN_H

#include "opengl_includes.h"

#include <QMap>

#include <fugio/pin_control_interface.h>
#include <fugio/opengl/state_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/pincontrolbase.h>
#include <fugio/core/variant_interface.h>

class StatePin : public fugio::PinControlBase, public fugio::OpenGLStateInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLStateInterface )

public:
	Q_INVOKABLE explicit StatePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~StatePin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const
	{
		return( "" );
	}

	virtual QString description( void ) const
	{
		return( "OpenGL State" );
	}

	virtual void loadSettings( QSettings & );

	virtual void saveSettings( QSettings & ) const;

	//-------------------------------------------------------------------------
	// InterfaceOpenGLState

	virtual void stateBegin( void );

	virtual void stateEnd( void );

	virtual void setFlag( int pFlag, bool pValue );

	virtual bool tstFlag( int pFlag ) const;

	virtual int blendRgbSrc( void ) const { return( mBlendRgbSrc ); }
	virtual int blendAlphaSrc( void ) const { return( mBlendAlphaSrc ); }

	virtual int blendRgbDst( void ) const { return( mBlendRgbDst ); }
	virtual int blendAlphaDst( void ) const { return( mBlendAlphaDst ); }

	virtual int blendRgbEquation( void ) const { return( mBlendRgbEquation ); }
	virtual int blendAlphaEquation( void ) const { return( mBlendAlphaEquation ); }

	virtual int depthFunc( void ) const { return( mDepthFunc ); }

	virtual int polygonMode( void ) const { return( mPolygonMode ); }

	virtual void setBlendRgbSrc( int pFactor ) { mBlendRgbSrc = pFactor; }
	virtual void setBlendAlphaSrc( int pFactor ) { mBlendAlphaSrc = pFactor; }

	virtual void setBlendRgbDst( int pFactor ) { mBlendRgbDst = pFactor; }
	virtual void setBlendAlphaDst( int pFactor ) { mBlendAlphaDst = pFactor; }

	virtual void setBlendRgbEquation( int pEquation ) { mBlendRgbEquation = pEquation; }
	virtual void setBlendAlphaEquation( int pEquation ) { mBlendAlphaEquation = pEquation; }

	virtual void setDepthFunction( int pFunction ) { mDepthFunc = pFunction; }

	virtual void setPolygonMode( int pMode ) { mPolygonMode = pMode; }

	//-------------------------------------------------------------------------

	inline static const QMap<QString,int> mapFlags( void )
	{
		return( mMapFlags );
	}

	template <class T> T input( QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( 0 );
		}

		return( qobject_cast<T>( pPin->connectedPin()->control()->qobject() ) );
	}

	QVariant variant( QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		fugio::VariantInterface	*V = input<fugio::VariantInterface *>( pPin );

		if( V )
		{
			return( V->variant() );
		}

		return( pPin->value() );
	}

private:
	QList<int>						 mFlags;

	static QMap<QString,int>		 mMapFlags;

	GLenum							 mBlendRgbSrc;
	GLenum							 mBlendAlphaSrc;

	GLenum							 mBlendRgbDst;
	GLenum							 mBlendAlphaDst;

	GLenum							 mBlendRgbEquation;
	GLenum							 mBlendAlphaEquation;

	GLenum							 mDepthFunc;

	GLenum							 mPolygonMode;
};

#endif // STATEPIN_H
