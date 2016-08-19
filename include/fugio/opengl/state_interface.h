#ifndef OPENGL_STATE_INTERFACE_H
#define OPENGL_STATE_INTERFACE_H

#include <QMatrix4x4>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class OpenGLStateInterface
{
public:
	virtual ~OpenGLStateInterface( void ) {}

    virtual void stateBegin( void ) = 0;

    virtual void stateEnd( void ) = 0;

    virtual void setFlag( int pFlag, bool pValue ) = 0;

    virtual bool tstFlag( int pFlag ) const = 0;

    virtual int blendRgbSrc( void ) const = 0;
    virtual int blendAlphaSrc( void ) const = 0;

    virtual int blendRgbDst( void ) const = 0;
    virtual int blendAlphaDst( void ) const = 0;

    virtual int blendRgbEquation( void ) const = 0;
    virtual int blendAlphaEquation( void ) const = 0;

    virtual int depthFunc( void ) const = 0;

    virtual int polygonMode( void ) const = 0;

    virtual void setBlendRgbSrc( int pFactor ) = 0;
    virtual void setBlendAlphaSrc( int pFactor ) = 0;

    virtual void setBlendRgbDst( int pFactor ) = 0;
    virtual void setBlendAlphaDst( int pFactor ) = 0;

    virtual void setBlendRgbEquation( int pEquation ) = 0;
    virtual void setBlendAlphaEquation( int pEquation ) = 0;

    virtual void setDepthFunction( int pFunction ) = 0;

    virtual void setPolygonMode( int pMode ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::OpenGLStateInterface, "com.bigfug.fugio.opengl.state/1.0" )

#endif // OPENGL_STATE_INTERFACE_H
