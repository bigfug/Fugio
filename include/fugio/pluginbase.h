#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QObject>
#include <QDebug>

#include "global.h"
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

FUGIO_NAMESPACE_BEGIN

class PluginBase : public QObject, public fugio::PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )

public:
	explicit PluginBase( QSharedPointer<fugio::GlobalInterface> pApp )
		: mApp( pApp )
	{
	}

	virtual ~PluginBase( void )
	{
	}

	virtual QObject *qobject( void )
	{
		return( this );
	}

protected:
	QSharedPointer<fugio::GlobalInterface>			 mApp;
};

FUGIO_NAMESPACE_END

#endif // PLUGINBASE_H
