#include "bufferpin.h"

#include "openglplugin.h"

BufferPin::BufferPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), mBuffer1( Q_NULLPTR ), mBuffer2( Q_NULLPTR ), mType( QMetaType::UnknownType ),
	  mStride( 0 ), mCount( 0 ), mSize( 0 ), mIndex( false ), mInstanced( false ),
	  mTarget( QOpenGLBuffer::VertexBuffer ), mDoubleBuffered( false )
{
}

BufferPin::~BufferPin()
{
	clear();
}

bool BufferPin::bind()
{
	if( !mBuffer1 || !mBuffer1->isCreated() )
	{
		return( false );
	}

	return( mBuffer1->bind() );
}

void BufferPin::release()
{
	if( mBuffer1 && mBuffer1->isCreated() )
	{
		mBuffer1->release();
	}
}

bool BufferPin::alloc( QMetaType::Type pType, int pSize, int pStride, int pCount, const void *pData )
{
	clear();

	if( !pStride )
	{
		pStride = QMetaType::sizeOf( pType ) * pSize;
	}

	if( !pCount || !pStride || !pSize )
	{
		return( false );
	}

	if( !mBuffer1 )
	{
		if( !( mBuffer1 = new QOpenGLBuffer( mTarget ) ) )
		{
			return( false );
		}

		mBuffer1->setUsagePattern( QOpenGLBuffer::StreamDraw );

		if( !mBuffer1->create() )
		{
			return( false );
		}
	}

	if( !mBuffer1->bind() )
	{
		return( false );
	}

	const int			ByteSize = pStride * pCount;

	if( pData )
	{
		mBuffer1->allocate( pData, ByteSize );
	}
	else
	{
		mBuffer1->allocate( ByteSize );
	}

	mBuffer1->release();

	mType   = pType;
	mStride = pStride;
	mCount  = pCount;
	mSize   = pSize;

	OPENGL_DEBUG( mPin->name() );

	if( mDoubleBuffered )
	{
		if( !mBuffer2 )
		{
			if( !( mBuffer2 = new QOpenGLBuffer( mTarget ) ) )
			{
				return( false );
			}

			if( !mBuffer2->create() )
			{
				return( false );
			}
		}

		if( !mBuffer2->bind() )
		{
			return( false );
		}

		if( pData )
		{
			mBuffer2->allocate( pData, ByteSize );
		}
		else
		{
			mBuffer2->allocate( ByteSize );
		}

		mBuffer2->release();
	}

	return( true );
}

void BufferPin::clear()
{
	if( mBuffer1 )
	{
		delete mBuffer1;

		mBuffer1 = Q_NULLPTR;
	}

	if( mBuffer2 )
	{
		delete mBuffer2;

		mBuffer2 = Q_NULLPTR;
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
