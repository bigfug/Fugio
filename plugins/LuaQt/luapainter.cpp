#include "luapainter.h"

#include <QImage>
#include <QDebug>
#include <QPainter>

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/lua/lua_interface.h>
#include <fugio/image/image.h>
#include <fugio/performance.h>

#include "luaqtplugin.h"
#include "luapen.h"
#include "luabrush.h"
#include "luacolor.h"
#include "luarectf.h"
#include "luapointf.h"
#include "luafont.h"
#include "luasizef.h"
#include "luafontmetrics.h"
#include "luagradient.h"
#include "luaimage.h"
#include "luatransform.h"
#include "lualine.h"

const char *LuaPainter::LuaPainterData::TypeName = "qt.painter";

#if defined( LUA_SUPPORTED )

const luaL_Reg LuaPainter::mLuaFunctions[] =
{
	{ 0, 0 }
};

const luaL_Reg LuaPainter::mLuaMethods[] =
{
	{ "__gc",				LuaPainter::luaDelete },
	{ "background",			LuaPainter::luaBackground },
	{ "brush",				LuaPainter::luaBrush },
	{ "drawArc",			LuaPainter::luaDrawArc },
	{ "drawEllipse",		LuaPainter::luaDrawEllipse },
	{ "drawImage",			LuaPainter::luaDrawImage },
	{ "drawLine",			LuaPainter::luaDrawLine },
	{ "drawPoint",			LuaPainter::luaDrawPoint },
	{ "drawRect",			LuaPainter::luaDrawRect },
	{ "drawText",			LuaPainter::luaDrawText },
	{ "eraseRect",			LuaPainter::luaEraseRect },
	{ "fillRect",			LuaPainter::luaFillRect },
	{ "finish",				LuaPainter::luaDelete },
	{ "font",				LuaPainter::luaFont },
	{ "pen",				LuaPainter::luaPen },
	{ "resetTransform",		LuaPainter::luaResetTransform },
	{ "rotate",				LuaPainter::luaRotate },
	{ "scale",				LuaPainter::luaScale },
	{ "setBackground",		LuaPainter::luaSetBackground },
	{ "setBrush",			LuaPainter::luaSetBrush },
	{ "setFont",			LuaPainter::luaSetFont },
	{ "setPen",				LuaPainter::luaSetPen },
	{ "setTransform",		LuaPainter::luaSetTransform },
	{ "shear",				LuaPainter::luaShear },
	{ "transform",			LuaPainter::luaTransform },
	{ "translate",			LuaPainter::luaTranslate },
	{ 0, 0 }
};

int LuaPainter::luaOpen( lua_State *L )
{
	luaL_newmetatable( L, LuaPainterData::TypeName );

	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);  /* duplicates the metatable */
	lua_setfield(L, -2, "__index");

	luaL_setfuncs( L, mLuaMethods, 0 );

	luaL_newlib( L, mLuaFunctions );

	return( 1 );
}

LuaPainter::LuaPainterData *LuaPainter::checkpainter( lua_State *L, int i )
{
	void *ud = luaL_checkudata( L, i, LuaPainterData::TypeName );

	luaL_argcheck( L, ud != NULL, i, "'painter' expected");

	return( (LuaPainterData *)ud );
}

int LuaPainter::luaNew( lua_State *L )
{
	LuaPainterData		*PainterData;

	if( LuaImage::isImage( L ) )
	{
		QImage			*Img = LuaImage::checkwritableimage( L );

		luaL_argcheck( L, Img, 1, "image is not valid or writable" );

		if( ( PainterData = (LuaPainterData *)lua_newuserdata( L, sizeof(LuaPainterData) ) ) == nullptr )
		{
			return( luaL_error( L, "Out of memory?" ) );
		}

		luaL_getmetatable( L, LuaPainterData::TypeName );
		lua_setmetatable( L, -2 );

		PainterData->mImage   = nullptr;
		PainterData->mPainter = new QPainter( Img );
		PainterData->mPin     = nullptr;

		if( PainterData->mPainter )
		{
			PainterData->mPainter->setRenderHint( QPainter::Antialiasing );
			PainterData->mPainter->setRenderHint( QPainter::TextAntialiasing );
		}

		return( 1 );
	}

	LuaInterface		*Lua  = LuaQtPlugin::lua();
	NodeInterface		*Node = Lua->node( L );
	PinInterface		*Pin  = Node->findPinByGlobalId( Lua->checkpin( L, 1 ) ).data();

	if( !Node || !Pin || !Pin->hasControl() )
	{
		return( 0 );
	}

	fugio::VariantInterface		*Image = qobject_cast<fugio::VariantInterface *>( Pin->control()->qobject() );

	if( !Image )
	{
		return( luaL_argerror( L, 1, "Need an image to paint onto" ) );
	}

	fugio::Image				DstImg = Image->variant().value<fugio::Image>();

	if( !DstImg.isValid() )
	{
		return( luaL_argerror( L, 1, "Image is not valid" ) );
	}

	if( ( PainterData = (LuaPainterData *)lua_newuserdata( L, sizeof(LuaPainterData) ) ) == nullptr )
	{
		return( luaL_error( L, "Out of memory?" ) );
	}

	luaL_getmetatable( L, LuaPainterData::TypeName );
	lua_setmetatable( L, -2 );

	PainterData->mImage   = new QImage( DstImg.internalBuffer( 0 ), DstImg.width(), DstImg.height(), DstImg.lineSize( 0 ), QImage::Format_ARGB32 );
	PainterData->mPainter = new QPainter( PainterData->mImage );
	PainterData->mPin     = Pin;

	if( PainterData->mPainter )
	{
		PainterData->mPainter->setRenderHint( QPainter::Antialiasing );
		PainterData->mPainter->setRenderHint( QPainter::TextAntialiasing );
	}

	return( 1 );
}

int LuaPainter::luaDelete( lua_State *L )
{
	LuaInterface		*Lua  = LuaQtPlugin::lua();
	NodeInterface		*Node = Lua->node( L );
	LuaPainterData		*PainterData = checkpainter( L );

	if( PainterData )
	{
		if( PainterData->mPainter )
		{
			if( PainterData->mPainter->isActive() )
			{
				PainterData->mPainter->end();

				if( PainterData->mPin )
				{
					Node->context()->pinUpdated( Node->findPinByGlobalId( PainterData->mPin->globalId() ) );
				}
			}

			delete PainterData->mPainter;

			PainterData->mPainter = nullptr;
		}

		if( PainterData->mImage )
		{
			delete PainterData->mImage;

			PainterData->mImage = nullptr;
		}
	}

	return( 0 );
}

int LuaPainter::luaEnd( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	LuaInterface		*Lua  = LuaQtPlugin::lua();
	NodeInterface		*Node = Lua->node( L );

	if( PainterData->mPin )
	{
		Node->context()->pinUpdated( Node->findPinByGlobalId( PainterData->mPin->globalId() ) );
	}

	return( 0 );
}

int LuaPainter::luaBrush( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	return( LuaBrush::pushbrush( L, PainterData->mPainter->brush() ) );
}

int LuaPainter::luaSetBrush( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	if( LuaBrush::isBrush( L, 2 ) )
	{
		QBrush			*Brush = LuaBrush::checkbrush( L, 2 );

		PainterData->mPainter->setBrush( *Brush );
	}
	else if( LuaColor::isColor( L, 2 ) )
	{
		QColor			 C = LuaColor::checkcolor( L, 2 );

		PainterData->mPainter->setBrush( C );
	}

	return( 0 );
}

int LuaPainter::luaBackground( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	return( LuaBrush::pushbrush( L, PainterData->mPainter->background() ) );
}

int LuaPainter::luaSetBackground( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	QBrush				*Brush = LuaBrush::checkbrush( L, 2 );

	if( Brush )
	{
		PainterData->mPainter->setBackground( *Brush );
	}

	return( 0 );
}

int LuaPainter::luaPen( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	return( LuaPen::pushpen( L, PainterData->mPainter->pen() ) );
}

int LuaPainter::luaSetPen( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	if( LuaPen::isPen( L, 2 ) )
	{
		QPen				*Pen = LuaPen::checkpen( L, 2 );

		if( Pen )
		{
			PainterData->mPainter->setPen( *Pen );
		}
	}
	else if( LuaColor::isColor( L, 2 ) )
	{
		QColor			C = LuaColor::checkcolor( L, 2 );

		PainterData->mPainter->setPen( C );
	}

	return( 0 );
}

int LuaPainter::luaFont( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	return( LuaFont::pushfont( L, PainterData->mPainter->font() ) );
}

int LuaPainter::luaSetFont( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	QFont				*Font = LuaFont::checkfont( L, 2 );

	if( Font )
	{
		PainterData->mPainter->setFont( *Font );
	}

	return( 0 );
}

int LuaPainter::luaDrawArc( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	bool				 RectOk;
	int					 NewTop = 2;
	QRectF				 Rect = LuaRectF::parseRectF( L, NewTop, &RectOk );

	if( !RectOk )
	{
		return( 0 );
	}

	if( lua_gettop( L ) - NewTop != 2 )
	{
		return( luaL_error( L, "Missing angle and length" ) );
	}

	qreal		Angle  = luaL_checknumber( L, NewTop + 0 );
	qreal		Length = luaL_checknumber( L, NewTop + 1 );

	PainterData->mPainter->drawArc( Rect, Angle, Length );

	return( 0 );
}

int LuaPainter::luaDrawEllipse( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	if( LuaPointF::isPointF( L, 2 ) && lua_gettop( L ) == 4 )
	{
		QPointF		C = LuaPointF::checkpointf( L, 2 );
		qreal		x = luaL_checknumber( L, 3 );
		qreal		y = luaL_checknumber( L, 4 );

		PainterData->mPainter->drawEllipse( C, x, y );

		return( 0 );
	}

	bool				 RectOk;
	int					 NewTop = 2;
	QRectF				 Rect = LuaRectF::parseRectF( L, NewTop, &RectOk );

	if( !RectOk )
	{
		return( 0 );
	}

	PainterData->mPainter->drawEllipse( Rect );

	return( 0 );
}

int LuaPainter::luaDrawImage( lua_State *L )
{	
//	drawImage(const QRectF &target, const QImage &image, const QRectF &source, Qt::ImageConversionFlags flags = Qt::AutoColor)
//	drawImage(const QPointF &point, const QImage &image, const QRectF &source, Qt::ImageConversionFlags flags = Qt::AutoColor)
//	drawImage(const QRectF &rectangle, const QImage &image)
//	drawImage(const QPointF &point, const QImage &image)
//	drawImage(int x, int y, const QImage &image, int sx = 0, int sy = 0, int sw = -1, int sh = -1, Qt::ImageConversionFlags flags = Qt::AutoColor)

	LuaPainterData		*PainterData = checkactivepainter( L );
	LuaInterface		*Lua  = LuaQtPlugin::lua();
	NodeInterface		*Node = Lua->node( L );
	int					 ImgArg = 3;

	QRectF				 Target;
	QRectF				 Source;

	if( lua_type( L, 2 ) == LUA_TUSERDATA )
	{
		Target = LuaRectF::checkrectf( L, 2 );

		if( lua_gettop( L ) >= 4 )
		{
			Source = LuaRectF::checkrectf( L, 4 );
		}
	}
	else
	{
		qreal			x1 = luaL_checknumber( L, 2 );
		qreal			y1 = luaL_checknumber( L, 3 );

		Target.setLeft( x1 );
		Target.setTop( y1 );

		ImgArg = 4;

		qreal			sx = 0;
		qreal			sy = 0;
		qreal			sw = -1;
		qreal			sh = -1;

		if( lua_gettop( L ) >= 5 )
		{
			sx = lua_tonumber( L, 5 );
		}

		if( lua_gettop( L ) >= 6 )
		{
			sy = lua_tonumber( L, 6 );
		}

		if( lua_gettop( L ) >= 7 )
		{
			sw = lua_tonumber( L, 7 );
		}

		if( lua_gettop( L ) >= 8 )
		{
			sh = lua_tonumber( L, 8 );
		}

		Source = QRectF( sx, sy, sw, sh );
	}

	fugio::VariantInterface					*I = nullptr;

	if( LuaImage::isImage( L, ImgArg ) )
	{
		QImage	*IM = LuaImage::checkimage( L, ImgArg );

		if( Source.isEmpty() )
		{
			Source.setBottom( IM->height() - 1 );
			Source.setRight( IM->width() - 1 );
		}

		if( Target.isEmpty() )
		{
			PainterData->mPainter->drawImage( Target.topLeft(), *IM, Source );
		}
		else
		{
			PainterData->mPainter->drawImage( Target, *IM, Source );
		}
	}
	else
	{
		PinInterface		*Pin  = Node->findPinByGlobalId( Lua->checkpin( L, ImgArg ) ).data();

		if( !Node || !Pin || !Pin->isConnected() )
		{
			return( 0 );
		}

		QSharedPointer<PinInterface>	P = Pin->connectedPin();

		if( !P || !P->hasControl() )
		{
			return( 0 );
		}

		I = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() );

		if( !I )
		{
			return( luaL_error( L, "Source is not an image" ) );
		}

		const QImage					 IM = I->variant().value<fugio::Image>().image();

		if( IM.isNull() )
		{
			return( luaL_error( L, "Source image is not valid" ) );
		}

		if( Source.isEmpty() )
		{
			Source.setBottom( IM.height() - 1 );
			Source.setRight( IM.width() - 1 );
		}

		if( Target.isEmpty() )
		{
			PainterData->mPainter->drawImage( Target.topLeft(), IM, Source );
		}
		else
		{
			PainterData->mPainter->drawImage( Target, IM, Source );
		}
	}

	return( 0 );
}

int LuaPainter::luaDrawLine( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	QLineF				 Line;

	if( LuaPointF::isPointF( L, 2 ) )
	{
		Line.setP1( LuaPointF::checkpointf( L, 2 ) );
		Line.setP2( LuaPointF::checkpointf( L, 3 ) );
	}
	else if( LuaLine::isLine( L, 2 ) )
	{
		Line = LuaLine::checkLine( L, 2 );
	}
	else
	{
		qreal		x1 = luaL_checknumber( L, 2 );
		qreal		y1 = luaL_checknumber( L, 3 );
		qreal		x2 = luaL_checknumber( L, 4 );
		qreal		y2 = luaL_checknumber( L, 5 );

		Line.setLine( x1, y1, x2, y2 );
	}

	PainterData->mPainter->drawLine( Line );

	return( 0 );
}

int LuaPainter::luaDrawPoint( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );
	QPointF				 P;

	if( LuaPointF::isPointF( L, 2 ) )
	{
		P = LuaPointF::checkpointf( L, 2 );
	}
	else
	{
		float		x = luaL_checknumber( L, 2 );
		float		y = luaL_checknumber( L, 3 );

		P = QPointF( x, y );
	}

	PainterData->mPainter->drawPoint( P );

	return( 0 );
}

int LuaPainter::luaDrawRect( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	bool				 RectOk;
	int					 NewTop = 2;
	QRectF				 Rect = LuaRectF::parseRectF( L, NewTop, &RectOk );

	if( !RectOk )
	{
		return( 0 );
	}

	PainterData->mPainter->drawRect( Rect );

	return( 0 );
}

int LuaPainter::luaDrawText( lua_State *L )
{
	const int LuaTop = lua_gettop( L );

	if( LuaTop < 3 )
	{
		luaL_error( L, "Not enough arguments" );
	}

	if( LuaTop == 3 )
	{
		// drawText(const QPointF &position, const QString &text)

		if( lua_type( L, 2 ) == LUA_TUSERDATA )
		{
			if( LuaPointF::isPointF( L, 2 ) )
			{
				QPointF		P = LuaPointF::checkpointf( L, 2 );

				drawText( L, P, 3 );
			}
			else
			{
				luaL_argerror( L, 2, "Point expected" );
			}
		}
		else
		{
			luaL_argerror( L, 2, "Unknown type" );
		}
	}
	else if( LuaTop == 4 )
	{
		// drawText(int x, int y, const QString &text)

		float			x = luaL_checknumber( L, 2 );
		float			y = luaL_checknumber( L, 3 );

		drawText( L, QPointF( x, y ), 4 );
	}
	else
	{
		luaL_error( L, "Too many arguments" );
	}

	return( 0 );
}

int LuaPainter::drawText( lua_State *L, const QPointF &pPoint, int i )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	if( lua_type( L, i ) != LUA_TSTRING )
	{
		luaL_argerror( L, i, "String expected" );
	}

	const char			*s = luaL_checkstring( L, i );

	QString				 S( s ? s : "" );

	if( S.isEmpty() )
	{
		return( 0 );
	}

	PainterData->mPainter->drawText( pPoint, S );

	return( 0 );
}

int LuaPainter::luaEraseRect( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	bool				 RectOk;
	int					 NewTop = 2;
	QRectF				 Rect = LuaRectF::parseRectF( L, NewTop, &RectOk );

	if( !RectOk )
	{
		return( 0 );
	}

	PainterData->mPainter->eraseRect( Rect );

	return( 0 );
}

int LuaPainter::luaFillRect(lua_State *L)
{
	LuaPainterData		*PainterData = checkactivepainter( L );
	QRectF				 Rect = LuaRectF::checkrectf( L, 2 );
	QColor				 Colour = LuaColor::checkcolor( L, 3 );

	PainterData->mPainter->fillRect( Rect, Colour );

	return( 0 );
}

int LuaPainter::luaResetTransform( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	PainterData->mPainter->resetTransform();

	return( 0 );
}

int LuaPainter::luaRotate( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );
	qreal				 Angle = luaL_checknumber( L, 2 );

	PainterData->mPainter->rotate( Angle );

	return( 0 );
}

int LuaPainter::luaScale( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );
	qreal				 X = luaL_checknumber( L, 2 );
	qreal				 Y = luaL_checknumber( L, 3 );

	PainterData->mPainter->scale( X, Y );

	return( 0 );
}

int LuaPainter::luaSetTransform( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );
	QTransform			*Transform = LuaTransform::checktransform( L, 2 );
	bool				 Combine = false;

	if( lua_gettop( L ) > 2 )
	{
		Combine = lua_toboolean( L, 3 );
	}

	PainterData->mPainter->setTransform( *Transform, Combine );

	return( 0 );
}

int LuaPainter::luaShear( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );
	qreal				 X = luaL_checknumber( L, 2 );
	qreal				 Y = luaL_checknumber( L, 3 );

	PainterData->mPainter->shear( X, Y );

	return( 0 );
}

int LuaPainter::luaTransform( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );

	LuaTransform::pushtransform( L, PainterData->mPainter->transform() );

	return( 1 );
}

int LuaPainter::luaTranslate( lua_State *L )
{
	LuaPainterData		*PainterData = checkactivepainter( L );
	QPointF				 P;

	if( LuaPointF::isPointF( L, 2 ) )
	{
		P = LuaPointF::checkpointf( L, 2 );
	}
	else
	{
		qreal				 X = luaL_checknumber( L, 2 );
		qreal				 Y = luaL_checknumber( L, 3 );

		P = QPointF( X, Y );
	}

	PainterData->mPainter->translate( P );

	return( 0 );
}

LuaPainter::LuaPainterData *LuaPainter::checkactivepainter( lua_State *L, int i )
{
	LuaPainterData		*PainterData = checkpainter( L, i );

	if( !PainterData->mPainter || !PainterData->mPainter->isActive() )
	{
		luaL_error( L, "Painter not active" );
	}

	return( PainterData );
}

#endif
