#ifndef INTERFACE_LUA_H
#define INTERFACE_LUA_H

#include <QUuid>
#include <QtPlugin>
#include <fugio/global.h>

struct lua_State;

#if !defined( lua_CFunction )
typedef int (*lua_CFunction) (lua_State *L);
#endif

FUGIO_NAMESPACE_BEGIN

#define IID_LUA				(QUuid("{222062CE-E3CE-4EF7-B962-C35A1F6F635C}"))

class NodeInterface;

class LuaExtension
{
public:
	virtual ~LuaExtension( void ) {}

	virtual void luaRegisterExtension( lua_State *L ) = 0;
};

//Q_DECLARE_INTERFACE( LuaExtension, "com.bigfug.fugio.lua.extension/1.0" )

class LuaInterface
{
public:
	virtual ~LuaInterface( void ) {}

	virtual void luaRegisterExtension( lua_CFunction pFunction ) = 0;

	virtual void luaUnregisterExtension( lua_CFunction pFunction ) = 0;

	virtual void luaRegisterLibrary( const char *pName, lua_CFunction pFunction ) = 0;

	virtual void luaAddFunction( const char *pName, lua_CFunction pFunction ) = 0;

	virtual void luaAddPinFunction( const QUuid &pPID, const char *pName, lua_CFunction pFunction ) = 0;

	virtual void luaAddExtensions( lua_State *L ) = 0;

	// methods callable from static Lua functions

	virtual fugio::NodeInterface *node( lua_State *L ) = 0;

	virtual QUuid checkpin( lua_State *L, int i ) = 0;

	virtual void pushpin( lua_State *L, const QUuid &pUuid ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::LuaInterface, "com.bigfug.fugio.lua/1.0" )

#endif // INTERFACE_LUA_H
