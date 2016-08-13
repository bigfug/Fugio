#include "luaplugin.h"

#include <QVector2D>
#include <QVector3D>

#include <fugio/global_interface.h>
#include <fugio/global_signals.h>

#include <fugio/nodecontrolbase.h>

#if defined( LUA_SUPPORTED )
#include <lua.hpp>
#endif

#include <fugio/lua/uuid.h>

#include "luanode.h"

#include "luaexpin.h"
#include "luaexnode.h"
#include "luaarray.h"

using namespace fugio;

QList<QUuid>	NodeControlBase::PID_UUID;

LuaPlugin		*LuaPlugin::mInstance = 0;

static const char NodeKey = 'k';

ClassEntry	NodeClasses[] =
{
	ClassEntry( "Lua", "Lua", NID_LUA, &LuaNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry PinClasses[] =
{
	ClassEntry()
};

#if defined( LUA_SUPPORTED )
void LuaPlugin::registerNodeToState( NodeInterface *N, lua_State *L ) const
{
	// Store a pointer to this in Lua's registry
	/* store a number */
	lua_pushlightuserdata( L, (void *)&NodeKey );  /* push address - only known to us */
	lua_pushlightuserdata( L, N );
	/* registry[&Key] = myNumber */
	lua_settable( L, LUA_REGISTRYINDEX );
}
#endif

PluginInterface::InitResult LuaPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( NodeClasses );

	mApp->registerPinClasses( PinClasses );

	mApp->registerInterface( IID_LUA, this );

#if defined( LUA_SUPPORTED )
	luaRegisterExtension( LuaExNode::lua_opennode );
	luaRegisterExtension( LuaExPin::lua_openpin );
	luaRegisterExtension( LuaArray::lua_openarray );

	luaAddFunction( 0, 0 );

	luaAddFunction( "log", LuaPlugin::luaLog );
	luaAddFunction( "timestamp", LuaPlugin::luaTimestamp );

	LuaNode::registerFunctions();
#endif

	return( INIT_OK );
}

void LuaPlugin::deinitialise( void )
{
	mApp->unregisterInterface( IID_LUA );

	mApp->unregisterPinClasses( PinClasses );

	mApp->unregisterNodeClasses( NodeClasses );

	mApp = 0;
}

void LuaPlugin::luaRegisterExtension( lua_CFunction pFunction )
{
	mExtensions.removeAll( pFunction );
	mExtensions.append( pFunction );
}

void LuaPlugin::luaUnregisterExtension( lua_CFunction pFunction )
{
	mExtensions.removeAll( pFunction );
}

void LuaPlugin::luaRegisterLibrary(const char *pName, int (*pFunction)(lua_State *))
{
	mLibFunctions.insert( pName, pFunction );
}

void LuaPlugin::luaAddExtensions( lua_State *L )
{
#if defined( LUA_SUPPORTED )
	for( lua_CFunction F : mExtensions )
	{
		F( L );

		lua_pop( L, 1 );
	}
#endif
}

void LuaPlugin::luaAddFunction(const char *pName, int (*pFunction)(lua_State *))
{
	mFunctions.prepend( luaL_Reg{ pName, pFunction } );
}

void LuaPlugin::luaAddPinFunction(const QUuid &pPID, const char *pName, int (*pFunction)(lua_State *))
{
	mPinFunctions.insertMulti( pPID, luaL_Reg{ pName, pFunction } );
}

void LuaPlugin::luaAddPinGet(const QUuid &pPID, LuaInterface::luaPinGetFunc pFunction)
{
	mGetFunctions.insert( pPID, pFunction );
}

NodeInterface *LuaPlugin::node( lua_State *L)
{
#if defined( LUA_SUPPORTED )
	lua_pushlightuserdata( L, (void *)&NodeKey );
	lua_gettable( L, LUA_REGISTRYINDEX );

	return( static_cast<NodeInterface *>( lua_touserdata( L, -1 ) ) );
#else
	return( nullptr );
#endif
}

QUuid LuaPlugin::checkpin( lua_State *L, int i)
{
#if defined( LUA_SUPPORTED )
	void *ud = luaL_checkudata( L, i, "fugio.pin" );

	luaL_argcheck( L, ud != NULL, i, "'pin' expected" );

	return( QUuid::fromRfc4122( QByteArray::fromRawData( (const char *)ud, 16 ) ) );
#else
	return( QUuid() );
#endif
}

void LuaPlugin::pushpin( lua_State *L, const QUuid &pUuid )
{
#if defined( LUA_SUPPORTED )
	quint8	*UD = (quint8 *)lua_newuserdata( L, 16 );

	if( UD )
	{
		luaL_getmetatable( L, "fugio.pin" );
		lua_setmetatable( L, -2 );

		memcpy( UD, pUuid.toRfc4122().data(), 16 );
	}
#endif
}

NodeInterface *LuaPlugin::getnode( lua_State *L )
{
	return( instance()->node( L ) );
}

QSharedPointer<PinInterface> LuaPlugin::getpin( lua_State *L, int i )
{
	LuaInterface						*LUA = instance();
	NodeInterface						*N = LUA->node( L );
	QUuid								 U = LUA->checkpin( L, i );

	return( N->findPinByGlobalId( U ) );
}

//-----------------------------------------------------------------------------

#if defined( LUA_SUPPORTED )
int LuaPlugin::luaLog( lua_State *L )
{
	for( int i = 1 ; i <= lua_gettop( L ) ; i++ )
	{
		qDebug() << QString( lua_tostring( L, i ) );
	}

	return( 0 );
}

int LuaPlugin::luaTimestamp( lua_State *L )
{
	LuaInterface					*LUA = qobject_cast<LuaInterface *>( LuaPlugin::instance()->app()->findInterface( IID_LUA ) );
	NodeInterface					*N = LUA->node( L );

	lua_pushinteger( L, N->context()->global()->timestamp() );

	return( 1 );
}

int LuaPlugin::pushVariant( lua_State *L, const QVariant &V )
{
	switch( QMetaType::Type( V.type() ) )
	{
		case QMetaType::QVariantList:
			{
				lua_newtable( L );

				const QVariantList		VL = V.toList();

				for( int i = 0 ; i < VL.size() ; i++ )
				{
					const QVariant		VI = VL.at( i );

					if( pushVariant( L, VI ) > 0 )
					{
						lua_rawseti( L, -2, i + 1 );	// Lua indexes start at 1
					}
				}

				return( 1 );
			}

		case QMetaType::QVariantMap:
			{
				lua_newtable( L );

				const QVariantMap		VL = V.toMap();
				const QStringList		VK = VL.keys();

				for( const QString &K : VK )
				{
					const QVariant		VI = VL[ K ];

					lua_pushfstring( L, "%s", K.toLatin1().data() );

					pushVariant( L, VI );

					lua_rawset( L, -3 );
				}

				return( 1 );
			}

		case QMetaType::QString:
			{
				lua_pushfstring( L, "%s", V.toString().toLatin1().data() );

				return( 1 );
			}

		case QMetaType::Bool:
			{
				lua_pushboolean( L, V.toBool() );

				return( 1 );
			}

		case QMetaType::Int:
			{
				lua_pushinteger( L, V.toInt() );

				return( 1 );
			}

		case QMetaType::Double:
			{
				lua_pushnumber( L, V.toDouble() );

				return( 1 );
			}

		case QMetaType::QByteArray:
			{
				const QByteArray		Data = V.toByteArray();

				lua_pushlstring( L, Data.data(), Data.size() );

				return( 1 );
			}

		case QMetaType::QVector2D:
			{
				lua_newtable( L );

				const QVector2D			Data = V.value<QVector2D>();

				lua_pushnumber( L, Data.x() );
				lua_rawseti( L, -2, 1 );

				lua_pushnumber( L, Data.y() );
				lua_rawseti( L, -2, 2 );

				return( 1 );
			}

		case QMetaType::QVector3D:
			{
				lua_newtable( L );

				const QVector3D			Data = V.value<QVector3D>();

				lua_pushnumber( L, Data.x() );
				lua_rawseti( L, -2, 1 );

				lua_pushnumber( L, Data.y() );
				lua_rawseti( L, -2, 2 );

				lua_pushnumber( L, Data.z() );
				lua_rawseti( L, -2, 3 );

				return( 1 );
			}

		case QMetaType::Float:
			{
				lua_pushnumber( L, V.toFloat() );

				return( 1 );
			}
			break;

		default:
			break;
	}

	return( 0 );
}

QVariant LuaPlugin::popVariant( lua_State *L, int idx )
{
	if( lua_type( L, idx ) == LUA_TTABLE )
	{
		QVariantList		VL;

		for( int i = 1 ; ; i++ )
		{
			lua_rawgeti( L, idx, i );

			if( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );

				break;
			}

			QVariant		V = popVariant( L, -1 );

			if( V.isValid() )
			{
				VL << V;
			}

			lua_pop( L, 1 );
		}

		return( VL );
	}

	if( lua_type( L, idx ) == LUA_TBOOLEAN )
	{
		return( QVariant::fromValue<bool>( lua_toboolean( L, idx ) ) );
	}

	if( lua_type( L, idx ) == LUA_TSTRING )
	{
		size_t		RawLen = lua_rawlen( L, idx );

		return( QString::fromLatin1( lua_tostring( L, idx ), RawLen ) );
	}

	if( lua_type( L, idx ) == LUA_TNUMBER )
	{
		return( QVariant::fromValue<double>( lua_tonumber( L, idx ) ) );
	}

	return( QVariant() );
}

#endif
