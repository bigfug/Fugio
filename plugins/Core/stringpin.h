#ifndef STRINGPIN_H
#define STRINGPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class StringPin : public fugio::PinControlBase, public fugio::VariantHelper<QString>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )
//	Q_PROPERTY( QString mValue READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "String" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit StringPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~StringPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE QString value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( const QString &pValue )
//	{
//		if( pValue != mValue )
//		{
//			mValue = pValue;

//			emit valueChanged( pValue );
//		}
//	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( mValues.first() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "String" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pValue );
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex, pOffset ) );
	}

	virtual QVariant variantSize( int pIndex = 0, int pOffset = 0 ) const Q_DECL_OVERRIDE
	{
		QString		SzeVar = variant( pIndex, pOffset ).value<QString>();

		return( SzeVar.length() );
	}

signals:
//	void valueChanged( const QString &pValue );

private:
//	QVector<QString>			mValues;
};

#endif // STRINGPIN_H
