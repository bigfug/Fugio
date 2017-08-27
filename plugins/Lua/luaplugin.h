#ifndef LUAPLUGIN_H
#define LUAPLUGIN_H

#include <QMultiMap>
#include <QVector>
#include <QMap>

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/plugin_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/text/syntax_highlighter_interface.h>
#include <fugio/text/syntax_highlighter_factory_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>

class LuaPlugin : public QObject, public fugio::PluginInterface, public fugio::LuaInterface, public fugio::SyntaxHighlighterFactoryInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.lua.plugin" )
	Q_INTERFACES( fugio::PluginInterface fugio::LuaInterface fugio::SyntaxHighlighterFactoryInterface )

public:
	Q_INVOKABLE explicit LuaPlugin( void );

	virtual ~LuaPlugin( void ) {}

	static LuaPlugin *instance( void )
	{
		return( mInstance );
	}

#if defined( LUA_SUPPORTED )
	void registerNodeToState( fugio::NodeInterface *N, lua_State *L ) const;
#endif

	fugio::GlobalInterface *app( void )
	{
		return( mApp );
	}

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceLua

	virtual void luaRegisterExtension( lua_CFunction pFunction ) Q_DECL_OVERRIDE;

	virtual void luaUnregisterExtension( lua_CFunction pFunction ) Q_DECL_OVERRIDE;

	virtual void luaRegisterLibrary( const char *pName, int (*pFunction) (lua_State *L) ) Q_DECL_OVERRIDE;

	virtual void luaAddExtensions( lua_State *L ) Q_DECL_OVERRIDE;

	virtual void luaAddFunction( const char *pName, int (*pFunction) (lua_State *L) ) Q_DECL_OVERRIDE;

	virtual void luaAddPinFunction( const QUuid &pPID, const char *pName, int (*pFunction) (lua_State *L) ) Q_DECL_OVERRIDE;

	virtual void luaAddPinGet( const QUuid &pPID, luaPinGetFunc pFunction ) Q_DECL_OVERRIDE;

	virtual void luaAddPinSet( const QUuid &pPID, luaPinSetFunc pFunction ) Q_DECL_OVERRIDE;

	virtual void luaAddPushVariantFunction( QMetaType::Type pType, luaPushVariantFunc pFunction ) Q_DECL_OVERRIDE;

	virtual void luaAddPopVariantFunction( QString pTypeName, luaPopVariantFunc pFunction ) Q_DECL_OVERRIDE;

	virtual fugio::NodeInterface *node( lua_State *L ) Q_DECL_OVERRIDE;

	virtual QUuid checkpin( lua_State *L, int i = 1 ) Q_DECL_OVERRIDE;

	virtual void pushpin( lua_State *L, const QUuid &pUuid ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------

	static fugio::NodeInterface *getnode( lua_State *L );

	static QSharedPointer<fugio::PinInterface> getpin( lua_State *L, int i = 1 );

	const QVector<luaL_Reg> &functions( void ) const
	{
		return( mFunctions );
	}

	const QList<luaL_Reg> pinFunctions( const QUuid &pPID ) const
	{
		return( mPinFunctions.values( pPID ) );
	}

	const QMap<const char *,lua_CFunction> &libFunctions( void ) const
	{
		return( mLibFunctions );
	}

	const QMap<QUuid,luaPinGetFunc> &getFunctions( void ) const
	{
		return( mGetFunctions );
	}

	const QMap<QUuid,luaPinSetFunc> &setFunctions( void ) const
	{
		return( mSetFunctions );
	}

	//-------------------------------------------------------------------------
	// SyntaxHighlighterFactoryInterface interface
public:
	virtual fugio::SyntaxHighlighterInstanceInterface *syntaxHighlighterInstance( QUuid pUuid ) const Q_DECL_OVERRIDE;

public:
#if defined( LUA_SUPPORTED )
	static int pushVariant( lua_State *L, const QVariant &V );

	static QVariant popVariant( lua_State *L, int idx );
#endif

private:
#if defined( LUA_SUPPORTED )
	static int luaLog( lua_State *L );

	static int luaTimestamp( lua_State *L );
#endif

private:
	static LuaPlugin							*mInstance;

	fugio::GlobalInterface						*mApp;

	QList<lua_CFunction>						 mExtensions;

	QVector<luaL_Reg>							 mFunctions;
	QMultiMap<QUuid,luaL_Reg>					 mPinFunctions;
	QMap<const char *,lua_CFunction>			 mLibFunctions;
	QMap<QUuid,luaPinGetFunc>					 mGetFunctions;
	QMap<QUuid,luaPinSetFunc>					 mSetFunctions;
	QMap<QMetaType::Type,luaPushVariantFunc>	 mPushFunctions;
	QMap<QString,luaPopVariantFunc>				 mPopFunctions;
};

#endif // LUAPLUGIN_H
