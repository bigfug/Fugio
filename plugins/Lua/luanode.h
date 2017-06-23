#ifndef LUANODE_H
#define LUANODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/lua_interface.h>
#include <fugio/text/syntax_error_interface.h>

typedef QMap<QString,lua_CFunction>		LuaMap;

class LuaNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Runs some Lua code" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Lua" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE LuaNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LuaNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	//

	static void registerFunctions( void );

	void parseErrors( QString pErrorText );

private:
#if defined( LUA_SUPPORTED )
	static int luaopen_fugio( lua_State *L );

	static int luaInput(lua_State *L);
	static int luaOutput(lua_State *L);
	static int luaInputs(lua_State *L);
	static int luaOutputs(lua_State *L);

	static int luaGet(lua_State *L);
	static int luaSet(lua_State *L);

	static int luaSetPath( lua_State* L, const char* path );
#endif

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputSource;
	fugio::SyntaxErrorInterface				*mValInputSource;

	lua_State								*mL;
	int										 mCompileStatus;
	int										 mCallStatus;

	static const luaL_Reg					 mLuaFunctions[];
	static const luaL_Reg					 mLuaMethods[];
};

#endif // LUANODE_H
