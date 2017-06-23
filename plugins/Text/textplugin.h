#ifndef TEXTPLUGIN_H
#define TEXTPLUGIN_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>
#include <fugio/text/syntax_highlighter_interface.h>

using namespace fugio;

class TextPlugin : public QObject, public fugio::PluginInterface, public fugio::SyntaxHighlighterInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface fugio::SyntaxHighlighterInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.text.plugin" )

public:
	explicit TextPlugin( void );

	virtual ~TextPlugin( void ) {}

	static fugio::GlobalInterface *global( void )
	{
		return( mApp );
	}

	static TextPlugin *instance( void )
	{
		return( mInstance );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// SyntaxHighlighterInterface interface
public:
	virtual void registerSyntaxHighlighter( const QUuid &pUuid, const QString &pName, SyntaxHighlighterFactoryInterface *pFactory ) Q_DECL_OVERRIDE;
	virtual void unregisterSyntaxHighlighter( const QUuid &pUuid ) Q_DECL_OVERRIDE;
	virtual SyntaxHighlighterFactoryInterface *syntaxHighlighterFactory( const QUuid &pUuid ) const Q_DECL_OVERRIDE;
	virtual QList<SyntaxHighlighterIdentity> syntaxHighlighters() const Q_DECL_OVERRIDE;
	virtual SyntaxHighlighterInstanceInterface *syntaxHighlighterInstance(const QUuid &pUuid) const Q_DECL_OVERRIDE;

private:
	static fugio::GlobalInterface	*mApp;
	static TextPlugin				*mInstance;

	typedef	QPair<QString,SyntaxHighlighterFactoryInterface *>	SyntaxHighlighterPair;

	QMap<QUuid,SyntaxHighlighterPair>		 mSyntaxHighlighterFactories;
};

#endif // TEXTPLUGIN_H
