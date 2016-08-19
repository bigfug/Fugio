#include "bufferentrypin.h"

BufferEntryPin::BufferEntryPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mSize( 0 ), mType( 0 ), mNormalised( GL_FALSE ), mTypeSize( 0 )
{

}

void BufferEntryPin::setType( GLenum pType )
{
	mType = pType;

	switch( mType )
	{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			mTypeSize = 1;
			break;

		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
#if !defined( Q_OS_RASPBERRY_PI )
		case GL_HALF_FLOAT:
#endif
			mTypeSize = 2;
			break;

		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
	case GL_FIXED:
#if !defined( Q_OS_RASPBERRY_PI )
		case GL_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
#endif
			mTypeSize = 4;
			break;

#if !defined( Q_OS_RASPBERRY_PI )
		case GL_DOUBLE:
			mTypeSize = 8;
			break;
#endif

		default:
			mTypeSize = 0;
			break;
	}
}

void BufferEntryPin::loadSettings(QSettings &)
{

}

void BufferEntryPin::saveSettings(QSettings &) const
{

}

const GLvoid *BufferEntryPin::bind( GLuint pIndex, GLsizei pStride, const GLvoid *pPointer )
{
	const GLvoid	*NextBuff = reinterpret_cast<const GLvoid *>( reinterpret_cast<const GLubyte *>( pPointer ) + entrySize() );

	glEnableVertexAttribArray( pIndex );

#if !defined( Q_OS_RASPBERRY_PI )
	if( GLEW_ARB_vertex_attrib_64bit )
	{
		switch( mType )
		{
			case GL_DOUBLE:
				glVertexAttribLPointer( pIndex, mSize, mType, pStride, pPointer );
				return( NextBuff );
		}
	}

	if( GLEW_VERSION_3_0 )
	{
		switch( mType )
		{
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			case GL_INT:
			case GL_UNSIGNED_INT:
				glVertexAttribIPointer( pIndex, mSize, mType, pStride, pPointer );
				return( NextBuff );
		}
	}
#endif

	glVertexAttribPointer( pIndex, mSize, mType, mNormalised, pStride, pPointer );

	return( NextBuff );
}

void BufferEntryPin::bind( GLuint pIndex )
{
	Q_UNUSED( pIndex )
}

void BufferEntryPin::release( GLuint pIndex )
{
	glDisableVertexAttribArray( pIndex );
}
