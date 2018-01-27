#ifndef MATHPLUGIN_H
#define MATHPLUGIN_H

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>
#include <fugio/math/math_interface.h>

using namespace fugio;

class MathPlugin : public QObject, public fugio::PluginInterface, public fugio::MathInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.math.plugin" )
	Q_INTERFACES( fugio::PluginInterface fugio::MathInterface )

public:
	Q_INVOKABLE explicit MathPlugin( void );

	virtual ~MathPlugin( void ) {}

	static MathPlugin *instance( void )
	{
		return( mInstance );
	}

	static fugio::GlobalInterface *app( void )
	{
		return( instance()->mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// MathInterface interface
public:
	virtual void registerMetaTypeMathOperator( QMetaType::Type pType, MathOperator pOperator, MathOperatorFunction pFunction) Q_DECL_OVERRIDE;
	virtual void deregisterMetaTypeMathOperator( QMetaType::Type pType, MathOperator pOperator ) Q_DECL_OVERRIDE;
	virtual MathOperatorFunction findMetaTypeMathOperator( QMetaType::Type pType, MathOperator pOperator ) const Q_DECL_OVERRIDE;

private:
	typedef QPair<QMetaType::Type,MathOperator>		 MathFuncPair;

	static MathPlugin								*mInstance;

	fugio::GlobalInterface							*mApp;

	QMap<MathFuncPair,MathOperatorFunction>			 mMathFunc;
};

#endif // MATHPLUGIN_H
