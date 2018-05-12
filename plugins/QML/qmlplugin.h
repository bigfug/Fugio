#ifndef QMLPLUGIN_H
#define QMLPLUGIN_H

#include <QObject>
#include <QDebug>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/nodecontrolbase.h>

#include <fugio/text/syntax_highlighter_factory_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>

using namespace fugio;

class QMLPlugin : public QObject, public fugio::PluginInterface, public fugio::SyntaxHighlighterFactoryInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface fugio::SyntaxHighlighterFactoryInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.qml.plugin" )

public:
	explicit QMLPlugin( void );

	virtual ~QMLPlugin( void ) {}

	static inline QMLPlugin *instance( void )
	{
		return( mInstance );
	}

	inline static GlobalInterface *app( void )
	{
		return( instance()->mApp );
	}

	//-------------------------------------------------------------------------
	// InterfacePlugin

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	SyntaxHighlighterInstanceInterface *syntaxHighlighterInstance( QUuid pUuid ) const;

private:
	static QMLPlugin				*mInstance;

	fugio::GlobalInterface			*mApp;

	static fugio::ClassEntry		 mNodeClasses[];
	static fugio::ClassEntry		 mPinClasses[];
};


#endif // QMLPLUGIN_H
