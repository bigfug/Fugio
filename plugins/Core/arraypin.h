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

	virtual ~ArrayPin( void ) Q_DECL_OVERRIDE;

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
	virtual void setType( QMetaType::Type pType ) Q_DECL_OVERRIDE
	{
		setVariantType( pType );
	}

	virtual QMetaType::Type type() const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	virtual void setStride( int pStride ) Q_DECL_OVERRIDE;

	virtual int stride() const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	virtual void setCount( int pCount ) Q_DECL_OVERRIDE;

	virtual int count() const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	virtual void reserve( int pCount ) Q_DECL_OVERRIDE
	{
		mReserve = pCount;
	}

	virtual void setElementCount(int pSize) Q_DECL_OVERRIDE
	{
		setVariantElementCount( pSize );
	}

	virtual int elementCount() const Q_DECL_OVERRIDE
	{
		return( mSize );
	}

	virtual void *array() Q_DECL_OVERRIDE;

	virtual const void *array() const Q_DECL_OVERRIDE;

	virtual void setArray( void *pArray ) Q_DECL_OVERRIDE
	{
		mData = pArray;

		if( mData )
		{
			if( mArray )
			{
				qFreeAligned( mArray );

				mArray = Q_NULLPTR;
			}
		}
		else
		{
			updateArray();
		}
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

public:
	virtual void setVariantType( QMetaType::Type pType ) Q_DECL_OVERRIDE;

	virtual QMetaType::Type variantType() const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	virtual QUuid variantPinControl() const Q_DECL_OVERRIDE
	{
		return( CorePlugin::instance()->app()->findPinForMetaType( mType ) );
	}

	virtual void setVariantCount(int pCount) Q_DECL_OVERRIDE;

	virtual int variantCount() const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	virtual void setVariantElementCount( int pElementCount ) Q_DECL_OVERRIDE;

	virtual int variantElementCount() const Q_DECL_OVERRIDE
	{
		return( mSize );
	}

	virtual void variantReserve( int pCount ) Q_DECL_OVERRIDE
	{
		mReserve = pCount;

		updateArray();
	}

	virtual void variantSetStride( int pStride ) Q_DECL_OVERRIDE;

	virtual int variantStride() const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	virtual int variantArraySize() const Q_DECL_OVERRIDE
	{
		return( mCount * mStride );
	}

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, 0, pValue );
	}

	virtual void setVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual QVariant variant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE;

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

	virtual QVariant variantSize( int pIndex = 0, int pOffset = 0 ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pIndex )
		Q_UNUSED( pOffset )

		return( variantElementCount() );
	}

	virtual int byteCount() const Q_DECL_OVERRIDE
	{
		return( mStride * mCount );
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
			pDataStream.writeRawData( reinterpret_cast<const char *>( mData ), variantArraySize() );
		}
		else if( mArray )
		{
			pDataStream.writeRawData( reinterpret_cast<const char *>( mArray ), variantArraySize() );
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

		if( mArray )
		{
			qFreeAligned( mArray );

			mArray = Q_NULLPTR;
		}

		mArray = qMallocAligned( variantArraySize(), 16 );

		if( mArray )
		{
			pDataStream.readRawData( reinterpret_cast<char *>( mArray ), variantArraySize() );
		}
	}

private:
	void updateArray( void );

private:
	void				*mData;
//	QByteArray			 mArray;
	void				*mArray;
	size_t				 mArraySize;
	QMetaType::Type		 mType;
	int					 mTypeSize;
	int					 mStride;
	int					 mCount;
	int					 mSize;
	int					 mReserve;
};

#endif // ARRAYPIN_H
