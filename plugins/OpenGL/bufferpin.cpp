#include "bufferpin.h"

#include "openglplugin.h"

BufferPin::BufferPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mBuffer1( 0 ), mBuffer2( 0 ), mType( QMetaType::UnknownType ), mStride( 0 ), mCount( 0 ), mSize( 0 ),
	  mIndex( false ), mInstanced( false ), mTarget( GL_ARRAY_BUFFER ), mDoubleBuffered( false )
{
}

BufferPin::~BufferPin()
{
	clear();
}

bool BufferPin::bind()
{
	if( !mBuffer1 )
	{
		return( false );
	}

	glBindBuffer( target(), mBuffer1 );

	OPENGL_DEBUG( mPin->name() );

	return( true );
}

void BufferPin::release()
{
	glBindBuffer( target(), 0 );
}

bool BufferPin::alloc(QMetaType::Type pType, int pSize, int pStride, int pCount, const void *pData )
{
	clear();

	if( !pSize )
	{
		pSize = QMetaType::sizeOf( pType );
	}

	if( !pCount || !pStride || !pSize )
	{
		return( false );
	}

	glGenBuffers( 1, &mBuffer1 );

	if( !mBuffer1 )
	{
		return( false );
	}

	const int			ByteSize = pStride * pCount;

	QVector<GLubyte>		MemClr;

	if( pData )
	{
		glBindBuffer( target(), mBuffer1 );

		glBufferData( target(), ByteSize, pData, GL_STREAM_DRAW );

		glBindBuffer( target(), 0 );
	}
	else
	{
		MemClr.resize( ByteSize );

		memset( MemClr.data(), 0, ByteSize );

		glBindBuffer( target(), mBuffer1 );

		glBufferData( target(), ByteSize, MemClr.constData(), GL_STREAM_DRAW );

		glBindBuffer( target(), 0 );
	}

	mType   = pType;
	mStride = pStride;
	mCount  = pCount;
	mSize   = pSize;

	OPENGL_DEBUG( mPin->name() );

	if( mDoubleBuffered )
	{
		glGenBuffers( 1, &mBuffer2 );

		if( !mBuffer2 )
		{
			return( false );
		}

		if( pData )
		{
			glBindBuffer( target(), mBuffer2 );

			glBufferData( target(), ByteSize, pData, GL_STREAM_DRAW );

			glBindBuffer( target(), 0 );
		}
		else
		{
			glBindBuffer( target(), mBuffer2 );

			glBufferData( target(), ByteSize, MemClr.constData(), GL_STREAM_DRAW );

			glBindBuffer( target(), 0 );
		}
	}

	return( true );
}

void BufferPin::clear()
{
	if( mBuffer1 )
	{
		glDeleteBuffers( 1, &mBuffer1 );

		mBuffer1 = 0;
	}

	if( mBuffer2 )
	{
		glDeleteBuffers( 1, &mBuffer2 );

		mBuffer2 = 0;
	}
}

void BufferPin::swapBuffers()
{
	std::swap( mBuffer1, mBuffer2 );
}

void BufferPin::loadSettings( QSettings &pSettings )
{
	mIndex = pSettings.value( "index", mIndex ).toBool();

	emit indexUpdated( mIndex );

	mInstanced = pSettings.value( "instanced", mInstanced ).toBool();

	emit indexUpdated( mInstanced );

	mDoubleBuffered = pSettings.value( "double-buffered", mDoubleBuffered ).toBool();
}

void BufferPin::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "index", mIndex );

	pSettings.setValue( "instanced", mInstanced );

	pSettings.setValue( "double-buffered", mDoubleBuffered );
}

int BufferPin::sizeDimensions() const
{
	return( 1 );
}

float BufferPin::size(int pDimension) const
{
	return( pDimension == 0 ? mCount : 0 );
}

float BufferPin::sizeWidth() const
{
	return( mCount );
}

float BufferPin::sizeHeight() const
{
	return( 0 );
}

float BufferPin::sizeDepth() const
{
	return( 0 );
}

QSizeF BufferPin::toSizeF() const
{
	return( QSizeF( mCount, 0 ) );
}

QVector3D BufferPin::toVector3D() const
{
	return( QVector3D( mCount, 0, 0 ) );
}
