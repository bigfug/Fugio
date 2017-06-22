#include "luanode.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <fugio/performance.h>

#include "luahighlighter.h"
#include "luaplugin.h"

using namespace fugio;

const luaL_Reg LuaNode::mLuaMethods[] =
{
	{ 0, 0 }
};

void LuaNode::registerFunctions()
{
#if defined( LUA_SUPPORTED )
	LuaPlugin		*LUA = LuaPlugin::instance();

	LUA->luaAddFunction( "input", LuaNode::luaInput );
	LUA->luaAddFunction( "output", LuaNode::luaOutput );
	LUA->luaAddFunction( "inputs", LuaNode::luaInputs );
	LUA->luaAddFunction( "outputs", LuaNode::luaOutputs );
#endif
}

LuaNode::LuaNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mL( nullptr ), mCompileStatus( LUA_ERRERR ), mCallStatus( LUA_ERRERR )
{
	mPinSource = pinInput( "Source" );

	mPinSource->setDescription( tr( "Lua source code" ) );

	mPinSource->registerInterface( IID_SYNTAX_HIGHLIGHTER_INSTANCE, new LuaHighlighter( this ) );
}

QList<QUuid> LuaNode::pinAddTypesInput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

QList<QUuid> LuaNode::pinAddTypesOutput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

bool LuaNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT || ( pPin->control() && qobject_cast<VariantInterface *>( pPin->control()->qobject() ) ) );
}

bool LuaNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	return( true );
}

bool LuaNode::deinitialise()
{
#if defined( LUA_SUPPORTED )
	if( mL )
	{
		lua_close( mL );

		mL = 0;
	}
#endif

	return( NodeControlBase::deinitialise() );
}

void LuaNode::inputsUpdated( qint64 pTimeStamp )
{
#if defined( LUA_SUPPORTED )
	fugio::Performance( node(), "inputsUpdated", pTimeStamp );

	if( mPinSource->isUpdated( pTimeStamp ) )
	{
		if( mL )
		{
			lua_close( mL );

			mL = 0;

			mCompileStatus = mCallStatus = LUA_ERRERR;
		}

		const QString	LuaSource = variant( mPinSource ).toString();

		if( LuaSource.isEmpty() )
		{
			return;
		}

		if( ( mL = luaL_newstate() ) == nullptr )
		{
			return;
		}

		//---------------------------------------------------------------------
		// Store a pointer to this in Lua's registry

		LuaPlugin::instance()->registerNodeToState( node().data(), mL );

		//---------------------------------------------------------------------
		// Load the Lua libraries (including our own)

		luaL_getsubtable( mL, LUA_REGISTRYINDEX, "_PRELOAD" );

		lua_pushcfunction( mL, luaopen_fugio );
		lua_setfield( mL, -2, "fugio" );

		const QMap<const char *,lua_CFunction>	&LibFncMap = LuaPlugin::instance()->libFunctions();

		for( auto it = LibFncMap.begin() ; it != LibFncMap.end() ; it++ )
		{
			lua_pushcfunction( mL, it.value() );
			lua_setfield( mL, -2, it.key() );
		}

		lua_pop( mL, 1 );  // remove _PRELOAD table

		luaL_openlibs( mL );

		//---------------------------------------------------------------------

		if( true )
		{
			QString		 DatDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );

			QString		 LuaUsr = QDir( DatDir ).absoluteFilePath( "Fugio/share/lua" );

			if( QFileInfo::exists( LuaUsr ) )
			{
				luaSetPath( mL, QDir::toNativeSeparators( LuaUsr.append( "/?.lua" ) ).toLatin1().constData() );
			}
		}

		if( true )
		{
#if defined( QT_DEBUG )
			QString		LuaInc = QDir::current().absoluteFilePath( "../Fugio/share/lua" );
#else
			QString		LuaInc = QDir::current().absoluteFilePath( "share/lua" );
#endif

			if( QFileInfo::exists( LuaInc ) )
			{
				luaSetPath( mL, QDir::toNativeSeparators( LuaInc.append( "/?.lua" ) ).toLatin1().constData() );
			}
		}

		//---------------------------------------------------------------------
		// load the code

		mCompileStatus = luaL_loadstring( mL, LuaSource.toLatin1().data() );

		if( mCompileStatus == LUA_OK )
		{
		}

		QString			 LuaErr;

		if( mCompileStatus == LUA_ERRSYNTAX )
		{
			LuaErr = lua_tolstring( mL, -1, 0 );

			mNode->setStatus( fugio::NodeInterface::Error );
			mNode->setStatusMessage( LuaErr );
		}

		QObject			*O = mPinSource->findInterface( IID_SYNTAX_HIGHLIGHTER_INSTANCE );

		if( LuaHighlighter *H = qobject_cast<LuaHighlighter *>( O ) )
		{
			if( !LuaErr.isEmpty() )
			{
				H->setErrors( LuaErr );
			}
			else
			{
				H->clearErrors();
			}
		}

		if( mCompileStatus == LUA_ERRMEM )
		{
			mNode->setStatus( fugio::NodeInterface::Error );
			mNode->setStatusMessage( "LUA_ERRMEM" );
		}

		if( mCompileStatus == LUA_ERRGCMM )
		{
			mNode->setStatus( fugio::NodeInterface::Error );
			mNode->setStatusMessage( "LUA_ERRGCMM" );
		}

		//---------------------------------------------------------------------
		// call the compiled code

		if( mCompileStatus == LUA_OK )
		{
			mCallStatus = lua_pcall( mL, 0, 0, 0 );

			if( mCallStatus == LUA_OK )
			{
				mNode->setStatus( fugio::NodeInterface::Initialised );
				mNode->setStatusMessage( QString() );
			}
			else
			{
				if( lua_isstring( mL, -1 ) )
				{
					QString	S( luaL_tolstring( mL, -1, 0 ) );

					mNode->setStatus( fugio::NodeInterface::Error );
					mNode->setStatusMessage( S );

					QObject			*O = mPinSource->findInterface( IID_SYNTAX_HIGHLIGHTER_INSTANCE );
					LuaHighlighter	*H = qobject_cast<LuaHighlighter *>( O );

					if( H )
					{
						H->setErrors( S );
					}
				}

				lua_pop( mL, 1 );
			}
		}
	}

	if( !pTimeStamp || !mL || mCompileStatus != LUA_OK || mCallStatus != LUA_OK )
	{
		return;
	}

	// we're calling the 'main' function so find it

	lua_getglobal( mL, "main" );

	// check we found the main function

	if( lua_isnil( mL, -1 ) )
	{
		lua_pop( mL, 1 );

		return;
	}

	// call the main function

	mCallStatus = lua_pcall( mL, 0, 0, 0 );

	if( mCallStatus != LUA_OK )
	{
		if( lua_isstring( mL, -1 ) )
		{
			QString		LuaErr( luaL_tolstring( mL, -1, 0 ) );

			QObject			*O = mPinSource->findInterface( IID_SYNTAX_HIGHLIGHTER_INSTANCE );

			if( LuaHighlighter *H = qobject_cast<LuaHighlighter *>( O ) )
			{
				if( !LuaErr.isEmpty() )
				{
					H->setErrors( LuaErr );
				}
				else
				{
					H->clearErrors();
				}
			}
		}

		lua_pop( mL, 1 );
	}
#endif
}

#if defined( LUA_SUPPORTED )
int LuaNode::luaInput( lua_State *L )
{
	LuaInterface					*LUA = qobject_cast<LuaInterface *>( LuaPlugin::instance()->app()->findInterface( IID_LUA ) );
	NodeInterface					*N = LUA->node( L );

	if( !N )
	{
		return( 0 );
	}

	const char	*Name = luaL_checklstring( L, 1, 0 );

	QSharedPointer<fugio::PinInterface>	P = N->findInputPinByName( QString( Name ) );

	if( !P )
	{
		lua_pushnil( L );
	}
	else
	{
		LUA->pushpin( L, P->globalId() );
	}

	return( 1 );
}

int LuaNode::luaOutput( lua_State *L )
{
	LuaInterface					*LUA = qobject_cast<LuaInterface *>( LuaPlugin::instance()->app()->findInterface( IID_LUA ) );
	NodeInterface					*N = LUA->node( L );

	if( !N )
	{
		return( 0 );
	}

	const char	*Name = luaL_checklstring( L, 1, 0 );

	QSharedPointer<fugio::PinInterface>	P = N->findOutputPinByName( QString( Name ) );

	if( !P )
	{
		lua_pushnil( L );
	}
	else
	{
		LUA->pushpin( L, P->globalId() );
	}

	return( 1 );
}

int LuaNode::luaInputs( lua_State *L )
{
	LuaInterface					*LUA = qobject_cast<LuaInterface *>( LuaPlugin::instance()->app()->findInterface( IID_LUA ) );
	NodeInterface					*N = LUA->node( L );

	if( !N )
	{
		return( 0 );
	}

	int		PinIdx = 1;	// Lua indexes start at 1

	lua_newtable( L );

	for( QSharedPointer<fugio::PinInterface> P : N->enumInputPins() )
	{
		if( P )
		{
			LUA->pushpin( L, P->globalId() );

			lua_rawseti( L, -2, PinIdx++ );
		}
	}

	return( 1 );
}

int LuaNode::luaOutputs( lua_State *L )
{
	LuaInterface					*LUA = qobject_cast<LuaInterface *>( LuaPlugin::instance()->app()->findInterface( IID_LUA ) );
	NodeInterface					*N = LUA->node( L );

	if( !N )
	{
		return( 0 );
	}

	int		PinIdx = 1;	// Lua indexes start at 1

	lua_newtable( L );

	for( QSharedPointer<fugio::PinInterface> P : N->enumOutputPins() )
	{
		if( P )
		{
			LUA->pushpin( L, P->globalId() );

			lua_rawseti( L, -2, PinIdx++ );
		}
	}

	return( 1 );
}

int LuaNode::luaSetPath( lua_State *L, const char *path )
{
	lua_getglobal( L, "package" );
	lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)
	std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack
	cur_path.append( ";" ); // do your path magic here
	cur_path.append( path );
	lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
	lua_pushstring( L, cur_path.c_str() ); // push the new one
	lua_setfield( L, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
	lua_pop( L, 1 ); // get rid of package table from top of stack
	return 0; // all done!
}

int LuaNode::luaopen_fugio( lua_State *L )
{
	LuaPlugin::instance()->luaAddExtensions( L );

	//-------------------------------------------------------------------------
	// Register fugio

	/* Create the metatable and put it on the stack. */
	luaL_newmetatable( L, "fugio" );

	/* Duplicate the metatable on the stack (We know have 2). */
	lua_pushvalue( L, -1 );

	/* Pop the first metatable off the stack and assign it to __index
	 * of the second one. We set the metatable for the table to itself.
	 * This is equivalent to the following in lua:
	 * metatable = {}
	 * metatable.__index = metatable
	 */
	lua_setfield( L, -2, "__index" );

	/* Set the methods to the metatable that should be accessed via object:func */
	luaL_setfuncs( L, mLuaMethods, 0 );

	/* Register the object.func functions into the table that is at the top of the
		 * stack. */
	//luaL_newlib( L, LuaPlugin::instance()->functions() );
	// manual expansion of lual_newlib to handle QVector

	const QVector<luaL_Reg> &LuaFuncs = LuaPlugin::instance()->functions();

	luaL_checkversion( L );
	lua_createtable( L, 0, LuaFuncs.size() - 1 );
	luaL_setfuncs( L, LuaFuncs.data(), 0 );

	return 1;
}
#endif
