#ifndef ARRAYPIN_H
#define ARRAYPIN_H

#include <QObject>

#include <QByteArray>
#include <QSettings>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/list_interface.h>
#include <fugio/core/size_interface.h>
#include <fugio/core/array_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

#include "coreplugin.h"

class ArrayPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::ArrayInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ArrayInterface fugio::VariantInterface fugio::SerialiseInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Array" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ArrayPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ArrayPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Array" );
	}

	//-------------------------------------------------------------------------
	// fugio::ArrayInterface

public:
	virtual void setType(QMetaType::Type pType) Q_DECL_OVERRIDE
	{
		mType = pType;
	}

	virtual QMetaType::Type type() const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	virtual void setStride(int pStride) Q_DECL_OVERRIDE
	{
		mStride = pStride;
	}

	virtual int stride() const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	virtual void setCount(int pCount) Q_DECL_OVERRIDE
	{
		mCount = pCount;
	}

	virtual int count() const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	virtual void reserve(int pCount) Q_DECL_OVERRIDE
	{
		mReserve = pCount;
	}

	virtual void setSize(int pSize) Q_DECL_OVERRIDE
	{
		mSize = pSize;
	}

	virtual int size() const Q_DECL_OVERRIDE
	{
		return( mSize );
	}

	virtual void *array() Q_DECL_OVERRIDE
	{
		if( mData )
		{
			return( mData );
		}

		mArray.resize( ( ( mStride * qMax( mCount, mReserve ) ) / sizeof( qlonglong ) ) + 1 );

		return( mArray.data() );

	}

	virtual const void *array() const Q_DECL_OVERRIDE
	{
		if( mData )
		{
			return( mData );
		}

		int		ArrayCount = ( ( mStride * qMax( mCount, mReserve ) ) / sizeof( qlonglong ) ) + 1;

		if( mArray.size() == ArrayCount )
		{
			return( mArray.constData() );
		}

		return( nullptr );
	}

	virtual void setArray(void *pArray) Q_DECL_OVERRIDE
	{
		mData = pArray;

		if( mData )
		{
			mArray.clear();
		}
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

public:
	virtual void setVariantType( QMetaType::Type pType ) Q_DECL_OVERRIDE
	{
		mType = pType;
	}

	virtual QMetaType::Type variantType() const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	virtual QUuid variantPinControl() const Q_DECL_OVERRIDE
	{
		return( CorePlugin::instance()->app()->findPinForMetaType( mType ) );
	}

	virtual void setVariantCount(int pCount) Q_DECL_OVERRIDE
	{
		mSize = pCount;
	}

	virtual int variantCount() const Q_DECL_OVERRIDE
	{
		return( mSize );
	}

	virtual void setVariantElementCount(int pElementCount) Q_DECL_OVERRIDE
	{
		mCount = pElementCount;
	}

	virtual int variantElementCount() const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	virtual void variantReserve(int pCount) Q_DECL_OVERRIDE
	{
		mReserve = pCount;
	}

	virtual void variantSetStride(int pStride) Q_DECL_OVERRIDE
	{
		mStride = pStride;
	}

	virtual int variantStride() const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	virtual int variantArraySize() const Q_DECL_OVERRIDE
	{
		return( mSize * mStride );
	}

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, 0, pValue );
	}

	virtual void setVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		quint8	*A = (quint8 *)( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

		if( !A )
		{
			return;
		}

		if( pIndex < 0 || pIndex >= mSize )
		{
			return;
		}

		if( pOffset < 0 || pOffset >= mCount )
		{
			return;
		}

		int				L = QMetaType::sizeOf( mType );

		A += ( L * pIndex ) + ( L * pOffset );

		QMetaType::construct( mType, A, pValue.constData() );
	}

	virtual QVariant variant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		const quint8	*A = (const quint8 *)( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

		if( !A )
		{
			return( QVariant() );
		}

		if( pIndex < 0 || pIndex >= mSize )
		{
			return( QVariant() );
		}

		if( pOffset < 0 || pOffset >= mCount )
		{
			return( QVariant() );
		}

		int				L = QMetaType::sizeOf( mType );

		A += ( L * pIndex ) + ( L * pOffset );

		QVariant		 V( mType, (const void *)A );

		return( V );
	}

	virtual void setFromBaseVariant(const QVariant &pValue) Q_DECL_OVERRIDE
	{
		setVariant( 0, 0, pValue );
	}

	virtual void setFromBaseVariant(int pIndex, const QVariant &pValue) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pOffset, pValue );
	}

	virtual QVariant baseVariant(int pIndex, int pOffset) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex, pOffset ) );
	}

	virtual void variantClear() Q_DECL_OVERRIDE
	{
	}

	virtual void variantAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pValue )
	}

	virtual void *variantArray() Q_DECL_OVERRIDE
	{
		return( array() );
	}

	virtual const void *variantArray() const Q_DECL_OVERRIDE
	{
		return( array() );
	}

	virtual void variantSetArray( void *pArray ) Q_DECL_OVERRIDE
	{
		setArray( pArray );
	}

	virtual QVariant variantSize( int pIndex = 0, int pOffset = 0 ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pIndex )
		Q_UNUSED( pOffset )

		return( variantElementCount() );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
		pDataStream << int( mType );
		pDataStream << mStride;
		pDataStream << mCount;
		pDataStream << mSize;

		if( mData )
		{
			pDataStream.writeRawData( static_cast<const char *>( mData ), mSize );
		}
		else
		{
			pDataStream.writeRawData( (const char *)mArray.data(), mSize );
		}
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		int			TmpInt;

		pDataStream >> TmpInt;
		pDataStream >> mStride;
		pDataStream >> mCount;
		pDataStream >> mSize;

		mType = QMetaType::Type( TmpInt );

		mArray.resize( mSize );

		pDataStream.readRawData( (char *)mArray.data(), mSize );
	}

private:
	void				*mData;
	QVector<qlonglong>	 mArray;		// 64-bit aligned
	QMetaType::Type		 mType;
	int					 mStride;
	int					 mCount;
	int					 mSize;
	int					 mReserve;
};

#endif // ARRAYPIN_H
