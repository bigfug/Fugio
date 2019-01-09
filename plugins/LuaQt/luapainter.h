#ifndef LUAPAINTER_H
#define LUAPAINTER_H

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <QVector>

#include <fugio/lua/lua_interface.h>

using namespace fugio;

class QPainter;

FUGIO_NAMESPACE_BEGIN
class PinInterface;
FUGIO_NAMESPACE_END

class LuaPainter
{
	typedef struct LuaPainterData
	{
		static const char *TypeName;

		QPainter		*mPainter;
		QImage			*mImage;
		PinInterface	*mPin;
	} LuaPainterData;

public:
	LuaPainter() {}

	virtual ~LuaPainter( void ) {}

#if defined( LUA_SUPPORTED )
	static void registerExtension( fugio::LuaInterface *LUA );

	static int luaOpen( lua_State *L );

	static int luaNew( lua_State *L );

private:
	static LuaPainterData *checkpainter( lua_State *L, int i = 1 );

	static int luaDelete( lua_State *L );
	static int luaEnd( lua_State *L );

	static int luaBrush( lua_State *L );
	static int luaSetBrush( lua_State *L );

	static int luaBackground( lua_State *L );
	static int luaSetBackground( lua_State *L );

	static int luaPen( lua_State *L );
	static int luaSetPen( lua_State *L );

	static int luaFont( lua_State *L );
	static int luaSetFont( lua_State *L );

	static int luaDrawArc( lua_State *L );
	static int luaDrawEllipse( lua_State *L );
	static int luaDrawImage( lua_State *L );
	static int luaDrawLine( lua_State *L );
	static int luaDrawPoint( lua_State *L );
	static int luaDrawRect( lua_State *L );
	static int luaDrawText( lua_State *L );

	static int luaEraseRect( lua_State *L );

	static int luaFillRect( lua_State *L );

	static int luaResetTransform( lua_State *L );
	static int luaRotate( lua_State *L );
	static int luaScale( lua_State *L );
	static int luaSetTransform( lua_State *L );
	static int luaShear( lua_State *L );
	static int luaTransform( lua_State *L );
	static int luaTranslate( lua_State *L );

private:
	static LuaPainterData *checkactivepainter( lua_State *L, int i = 1 );

	static int drawText( lua_State *L, const QPointF &pPoint, int i );

	static const luaL_Reg					mLuaFunctions[];
	static const luaL_Reg					mLuaMethods[];
#endif
};

#endif // LUAPAINTER_H
