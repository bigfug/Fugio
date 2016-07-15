#ifndef PAINTERPLUGIN_H
#define PAINTERPLUGIN_H

#include <fugio/plugin_interface.h>
#include <fugio/lua/lua_interface.h>

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

using namespace fugio;

class LuaQtPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.lua-qt.plugin" )
	Q_INTERFACES( fugio::PluginInterface )

public:
	Q_INVOKABLE explicit LuaQtPlugin( void ) { mInstance = this; }

	virtual ~LuaQtPlugin( void ) {}

	static LuaQtPlugin *instance( void )
	{
		return( mInstance );
	}

	static LuaInterface *lua( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp ) Q_DECL_OVERRIDE;

	virtual void deinitialise( void ) Q_DECL_OVERRIDE;

private:
	static int luaOpen( lua_State *L );

//	static int luaImageRect( lua_State *L );
//	static int luaImageResize( lua_State *L );
//	static int luaImageSize( lua_State *L );

//	static int luaGetImage( const QUuid &pPinId, lua_State *L );

private:
	static LuaQtPlugin			*mInstance;

	static const luaL_Reg		 mLuaFunctions[];
	static const luaL_Reg		 mLuaMethods[];

	static GlobalInterface		*mApp;
};


#endif // PAINTERPLUGIN_H
