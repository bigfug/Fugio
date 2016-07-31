#ifndef FLOAT_H
#define FLOAT_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class FloatNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "A floating point number in a GUI." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Float" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit FloatNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FloatNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface
	
	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void textUpdated( const QString &pText );

protected slots:
	void textChanged( const QString &pText );

private:
	QSharedPointer<fugio::PinInterface>			 mPinValue;
	fugio::VariantInterface						*mFloat;
};

#endif // FLOAT_H
