#ifndef JOINPIN_H
#define JOINPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>

#include <fugio/osc/join_interface.h>

class JoinPin : public fugio::PinControlBase, public fugio::osc::JoinInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::osc::JoinInterface )

public:
	Q_INVOKABLE explicit JoinPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~JoinPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "OSC Join" );
	}

	//-------------------------------------------------------------------------
	// SplitInterface interface
public:
	virtual void oscJoin( QStringList pPath, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual void oscPath( QStringList &pPath ) const Q_DECL_OVERRIDE;
};

#endif // JOINPIN_H
