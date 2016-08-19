#ifndef BUFFERPIN_H
#define BUFFERPIN_H

#include "opengl_includes.h"

#include <QObject>

#include <QSettings>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>
#include <fugio/opengl/buffer_interface.h>

class BufferPin : public fugio::PinControlBase, public fugio::OpenGLBufferInterface, public fugio::SizeInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLBufferInterface fugio::SizeInterface )

public:
	Q_INVOKABLE explicit BufferPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~BufferPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString::number( mBuffer1 ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "OpenGL Buffer" );
	}

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual bool bind( void ) Q_DECL_OVERRIDE;

	virtual void release( void ) Q_DECL_OVERRIDE;

	virtual bool alloc( QMetaType::Type pType, int pSize, int pStride, int pCount, const void *pData = 0 ) Q_DECL_OVERRIDE;

	virtual void clear( void ) Q_DECL_OVERRIDE;

	virtual inline GLuint buffer( void ) const Q_DECL_OVERRIDE
	{
		return( mBuffer1 );
	}

	virtual inline GLuint target( void ) const Q_DECL_OVERRIDE
	{
		return( mIndex ? GL_ELEMENT_ARRAY_BUFFER : mTarget );
	}

	virtual int count( void ) const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	virtual QMetaType::Type type( void ) const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	virtual int stride( void ) const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	virtual int size( void ) const Q_DECL_OVERRIDE
	{
		return( mSize );
	}

	virtual bool instanced( void ) const Q_DECL_OVERRIDE
	{
		return( mInstanced );
	}

	virtual void setIndex( bool pIndex ) Q_DECL_OVERRIDE
	{
		mIndex = pIndex;
	}

	virtual void setInstanced( bool pInstanced ) Q_DECL_OVERRIDE
	{
		mInstanced = pInstanced;
	}

	virtual void setTarget( GLuint pTarget ) Q_DECL_OVERRIDE
	{
		mTarget = pTarget;
	}

	virtual void setDoubleBuffered( bool pDoubleBuffered ) Q_DECL_OVERRIDE
	{
		mDoubleBuffered = pDoubleBuffered;
	}

	virtual bool isDoubleBuffered( void ) const Q_DECL_OVERRIDE
	{
		return( mDoubleBuffered );
	}

	virtual void swapBuffers( void ) Q_DECL_OVERRIDE;

	virtual GLuint srcBuf( void ) const Q_DECL_OVERRIDE
	{
		return( mBuffer1 );
	}

	virtual GLuint dstBuf( void ) const Q_DECL_OVERRIDE
	{
		return( mBuffer2 );
	}

	// SizeInterface interface
public:
	virtual int sizeDimensions() const Q_DECL_OVERRIDE;
	virtual float size(int pDimension) const Q_DECL_OVERRIDE;
	virtual float sizeWidth() const Q_DECL_OVERRIDE;
	virtual float sizeHeight() const Q_DECL_OVERRIDE;
	virtual float sizeDepth() const Q_DECL_OVERRIDE;
	virtual QSizeF toSizeF() const Q_DECL_OVERRIDE;
	virtual QVector3D toVector3D() const Q_DECL_OVERRIDE;

signals:
	void indexUpdated( bool pValue );
	void instancedUpdated( bool pValue );

private:
	GLuint					 mBuffer1;
	GLuint					 mBuffer2;
	QMetaType::Type			 mType;
	int						 mStride;
	int						 mCount;
	int						 mSize;
	bool					 mIndex;
	bool					 mInstanced;
	GLuint					 mTarget;
	//GLuint					 mBufferOffset;
	bool					 mDoubleBuffered;
};

#endif // BUFFERPIN_H
