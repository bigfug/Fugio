#ifndef NAMESPACEPIN_H
#define NAMESPACEPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>
#include <fugio/node_control_interface.h>

#include <fugio/osc/namespace_interface.h>

class NamespacePin : public fugio::PinControlBase, public fugio::osc::NamespaceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::osc::NamespaceInterface )

public:
	Q_INVOKABLE explicit NamespacePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~NamespacePin( void ) {}

	void insert( const QString &S, const QVariant &V )
	{
		mDataNames.insert( S, V );
	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "OSC Namespace" );
	}

	// NamespaceInterface interface
public:
	virtual QStringList oscNamespace() Q_DECL_OVERRIDE;

	virtual QList<fugio::NodeControlInterface::AvailablePinEntry> oscPins( const QStringList &pCurDir ) const Q_DECL_OVERRIDE;

private:
	QHash<QString,QVariant>						 mDataNames;
};

#endif // NAMESPACEPIN_H
