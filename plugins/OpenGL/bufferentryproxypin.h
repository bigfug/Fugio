#ifndef BUFFERENTRYPROXYPIN_H
#define BUFFERENTRYPROXYPIN_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/pin_control_interface.h>
#include <fugio/opengl/buffer_entry_interface.h>

#include <fugio/pincontrolbase.h>

class BufferEntryProxyPin : public fugio::PinControlBase, public fugio::OpenGLBufferEntryInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLBufferEntryInterface )

public:
	Q_INVOKABLE explicit BufferEntryProxyPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~BufferEntryProxyPin( void ) {}

//	void setSize( GLint pSize )
//	{
//		mSize = pSize;
//	}

//	void setType( GLenum pType );

//	void setNormalised( GLboolean pNormalised )
//	{
//		mNormalised = pNormalised;
//	}

	void setBufferOffset( GLuint pBufferOffset )
	{
		mBufferOffset = pBufferOffset;
	}

	void setBufferInput( QSharedPointer<fugio::PinInterface> pPin )
	{
		mBufferPin = pPin;
	}

	void setBufferEntryInput( QSharedPointer<fugio::PinInterface> pPin )
	{
		mBufferEntryPin = pPin;
	}

	void setBufferStride( GLuint pBufferStride )
	{
		mBufferStride = pBufferStride;
	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Buffer Entry Proxy" );
	}

	//-------------------------------------------------------------------------
	// fugio::OpenGLBufferEntryInterface

public:
	virtual const GLvoid *bind( GLuint pIndex, GLsizei pStride, const GLvoid *pPointer ) Q_DECL_OVERRIDE;

	virtual void bind( GLuint pIndex ) Q_DECL_OVERRIDE;

	virtual void release( GLuint pIndex ) Q_DECL_OVERRIDE;

	virtual GLuint typeSize( void ) const Q_DECL_OVERRIDE;

	virtual GLuint entrySize( void ) const Q_DECL_OVERRIDE;

	virtual bool normalised( void ) const Q_DECL_OVERRIDE;

	virtual void setSize( GLint pSize ) Q_DECL_OVERRIDE;

	virtual void setType( GLenum pType ) Q_DECL_OVERRIDE;

	virtual void setNormalised( GLboolean pNormalised ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>		mBufferPin;
	QSharedPointer<fugio::PinInterface>		mBufferEntryPin;
	GLuint									mBufferOffset;
	GLuint									mBufferStride;
};

#endif // BUFFERENTRYPROXYPIN_H
