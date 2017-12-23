#ifndef COLOURLISTPIN_H
#define COLOURLISTPIN_H

#include <QObject>
#include <QColor>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/list_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class ColourListPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface fugio::ListInterface )
	Q_PROPERTY( QVector<QColor> value READ value WRITE setValue NOTIFY valueChanged )

public:
	Q_INVOKABLE explicit ColourListPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ColourListPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

	Q_INVOKABLE QVector<QColor> value( void ) const
	{
		return( mValue );
	}

	Q_INVOKABLE void setValue( const QVector<QColor> &pValue )
	{
		if( pValue != mValue )
		{
			mValue = pValue;

			emit valueChanged( pValue );
		}
	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "List of %1 colours" ).arg( mValue.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "ColourList" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pValue )

		//setValue( pValue.toStringList() );
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		QVariantList		VarLst;

		for( QColor C : mValue )
		{
			QVariant	V = C;

			VarLst << V;
		}

		return( VarLst );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pValue );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		return( variant() );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
		pDataStream << mValue;
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		QVector<QColor>		NewVal;

		pDataStream >> NewVal;

		setValue( NewVal );
	}

	//-------------------------------------------------------------------------
	// fugio::ListInterface

	virtual int listSize() const Q_DECL_OVERRIDE
	{
		return( mValue.size() );
	}

	virtual QUuid listPinControl( void ) const Q_DECL_OVERRIDE;

	virtual QVariant listIndex( int pIndex ) const Q_DECL_OVERRIDE
	{
		QVariant		V = mValue.at( pIndex );

		return( V );
	}

	virtual void listSetIndex( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QColor		C = pValue.value<QColor>();

		mValue[ pIndex ] = C;
	}

	virtual void listSetSize( int pSize ) Q_DECL_OVERRIDE
	{
		mValue.resize( pSize );
	}

	virtual void listClear() Q_DECL_OVERRIDE
	{
		mValue.clear();
	}

	virtual void listAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue << pValue.value<QColor>();
	}

	virtual bool listIsEmpty() const Q_DECL_OVERRIDE
	{
		return( mValue.isEmpty() );
	}

	virtual QMetaType::Type listType( void ) const Q_DECL_OVERRIDE
	{
		return( QMetaType::QColor );
	}

signals:
	void valueChanged( const QVector<QColor> &pValue );

private:
	QVector<QColor>		mValue;
};


#endif // COLOURLISTPIN_H
