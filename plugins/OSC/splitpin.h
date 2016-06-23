#ifndef SPLITPIN_H
#define SPLITPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>

#include <fugio/osc/split_interface.h>
#include <fugio/osc/namespace_interface.h>

class SplitPin : public fugio::PinControlBase, public fugio::osc::SplitInterface, public fugio::osc::NamespaceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::osc::SplitInterface fugio::osc::NamespaceInterface )

public:
	Q_INVOKABLE explicit SplitPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~SplitPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "OSC Split" );
	}

	//-------------------------------------------------------------------------
	// SplitInterface interface
public:
	virtual void oscSplit( QStringList pPath, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual void oscPath( QStringList &pPath ) const Q_DECL_OVERRIDE;


	//-------------------------------------------------------------------------
	// NamespaceInterface interface
public:
	virtual QStringList oscNamespace() Q_DECL_OVERRIDE;
	virtual QList<fugio::NodeControlInterface::AvailablePinEntry> oscPins(const QStringList &pCurDir) const Q_DECL_OVERRIDE;
};

#endif // SPLITPIN_H
