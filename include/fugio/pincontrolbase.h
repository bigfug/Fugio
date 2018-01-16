#ifndef PINCONTROLBASE_H
#define PINCONTROLBASE_H

#include <QObject>
#include <QDebug>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#define WIKI_PIN_URL(x)		"http://wiki.bigfug.com/view/Pins:" ## x

FUGIO_NAMESPACE_BEGIN

class PinControlBase : public QObject, public fugio::PinControlInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PinControlInterface )

public:
	explicit PinControlBase( QSharedPointer<fugio::PinInterface> pPin )
		: mPin( pPin )
	{
	}

	virtual ~PinControlBase( void )
	{
	}

	inline virtual QObject *qobject( void ) Q_DECL_OVERRIDE Q_DECL_FINAL
	{
		return( this );
	}

	inline virtual const QObject *qobject( void ) const Q_DECL_OVERRIDE Q_DECL_FINAL
	{
		return( this );
	}

	inline virtual QWidget *gui( void ) Q_DECL_OVERRIDE
	{
		return( Q_NULLPTR );
	}

	inline  virtual void loadSettings( QSettings & ) Q_DECL_OVERRIDE
	{

	}

	inline virtual void saveSettings( QSettings & ) const Q_DECL_OVERRIDE
	{

	}

	inline virtual void dataExport( QTextStream & ) const Q_DECL_OVERRIDE
	{
	}

	inline virtual void dataImport( QTextStream & ) Q_DECL_OVERRIDE
	{
	}

	inline virtual QString toString( void ) const Q_DECL_OVERRIDE = 0;

	inline virtual QString description( void ) const Q_DECL_OVERRIDE = 0;

	template <class T> T pinValue( void ) const
	{
		return( mPin->value().value<T>() );
	}

	inline virtual QSharedPointer<fugio::PinInterface> pin() const Q_DECL_OVERRIDE Q_DECL_FINAL
	{
		return( mPin );
	}

protected:
	QSharedPointer<fugio::PinInterface>			 mPin;

private:
	Q_DISABLE_COPY( PinControlBase )
};

FUGIO_NAMESPACE_END

#endif // PINCONTROLBASE_H
