#ifndef BUFFERENTRYPIN_H
#define BUFFERENTRYPIN_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/pin_control_interface.h>
#include <fugio/opengl/buffer_entry_interface.h>

#include <fugio/pincontrolbase.h>

class BufferEntryPin : public fugio::PinControlBase, public fugio::OpenGLBufferEntryInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::OpenGLBufferEntryInterface )

public:
	Q_INVOKABLE explicit BufferEntryPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~BufferEntryPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Buffer Entry" );
	}

	virtual void loadSettings( QSettings & ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings & ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::OpenGLBufferEntryInterface

public:
	virtual const GLvoid *bind( GLuint pIndex, GLsizei pStride, const GLvoid *pPointer ) Q_DECL_OVERRIDE;

	virtual void bind( GLuint pIndex ) Q_DECL_OVERRIDE;

	virtual void release( GLuint pIndex ) Q_DECL_OVERRIDE;

	virtual GLuint typeSize( void ) const Q_DECL_OVERRIDE
	{
		return( mTypeSize );
	}

	virtual GLuint entrySize( void ) const Q_DECL_OVERRIDE
	{
		return( mTypeSize * mSize );
	}

	virtual bool normalised( void ) const Q_DECL_OVERRIDE
	{
		return( mNormalised );
	}

	virtual void setSize( GLint pSize ) Q_DECL_OVERRIDE
	{
		mSize = pSize;
	}

	virtual void setType( GLenum pType ) Q_DECL_OVERRIDE;

	virtual void setNormalised( GLboolean pNormalised ) Q_DECL_OVERRIDE
	{
		mNormalised = pNormalised;
	}

private:
	GLint			mSize;
	GLenum			mType;
	GLboolean		mNormalised;
	GLuint			mTypeSize;
};


#endif // BUFFERENTRYPIN_H
