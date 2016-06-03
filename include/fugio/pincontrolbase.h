#ifndef PINCONTROLBASE_H
#define PINCONTROLBASE_H

#include <QObject>
#include <QDebug>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

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

    virtual QObject *qobject( void ) Q_DECL_OVERRIDE
    {
        return( this );
    }

    virtual const QObject *qobject( void ) const Q_DECL_OVERRIDE
    {
        return( this );
    }

    virtual QWidget *gui( void ) Q_DECL_OVERRIDE
    {
        return( 0 );
    }

    virtual void loadSettings( QSettings & ) Q_DECL_OVERRIDE
    {

    }

	virtual void saveSettings( QSettings & ) const Q_DECL_OVERRIDE
    {

    }

    virtual void dataExport( QTextStream & ) const Q_DECL_OVERRIDE
    {
    }

    virtual void dataImport( QTextStream & ) Q_DECL_OVERRIDE
    {
    }

	virtual QString toString( void ) const Q_DECL_OVERRIDE = 0;

	virtual QString description( void ) const Q_DECL_OVERRIDE = 0;

    template <class T> T pinValue( void ) const
    {
        return( mPin->value().value<T>() );
    }

protected:
    QSharedPointer<fugio::PinInterface>			 mPin;

private:
	Q_DISABLE_COPY( PinControlBase )
};

FUGIO_NAMESPACE_END

#endif // PINCONTROLBASE_H
