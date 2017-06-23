#include "syntaxerrorpin.h"

#include <fugio/core/uuid.h>

#include <QRect>
#include <QRectF>

QDataStream &operator <<( QDataStream &DS, const fugio::SyntaxError &SE )
{
	DS << SE.mLineStart << SE.mLineEnd << SE.mColumnStart << SE.mColumnEnd << SE.mError;

	return( DS );
}

QDataStream &operator >>( QDataStream &DS, fugio::SyntaxError &SE )
{
	DS >> SE.mLineStart >> SE.mLineEnd >> SE.mColumnStart >> SE.mColumnEnd >> SE.mError;

	return( DS );
}

fugio::SyntaxErrorSignals::SyntaxErrorSignals( void )
{

}

fugio::SyntaxErrorSignals::~SyntaxErrorSignals( void )
{

}

SyntaxErrorPin::SyntaxErrorPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

void SyntaxErrorPin::registerMetaType()
{
	qRegisterMetaTypeStreamOperators<fugio::SyntaxError>( "fugio::SyntaxError" );
}

QString SyntaxErrorPin::toString() const
{
	return( QString( "Syntax Errors: %1" ).arg( mSyntaxErrors.size() ) );
}

void SyntaxErrorPin::serialise( QDataStream &pDataStream ) const
{
	pDataStream << mSyntaxErrors;
}

void SyntaxErrorPin::deserialise( QDataStream &pDataStream )
{
	pDataStream >> mSyntaxErrors;
}

int SyntaxErrorPin::listSize() const
{
	return( mSyntaxErrors.size() );
}

QUuid SyntaxErrorPin::listPinControl() const
{
	return( QUuid() );
}

QVariant SyntaxErrorPin::listIndex( int pIndex ) const
{
	QVariant		 V;

	if( pIndex < 0 || pIndex >= mSyntaxErrors.size() )
	{
		return( V );
	}

	V.setValue( mSyntaxErrors.at( pIndex ) );

	return( V );
}

void SyntaxErrorPin::listSetIndex( int pIndex, const QVariant &pValue )
{
	fugio::SyntaxError	SE  = pValue.value<fugio::SyntaxError>();

	if( mSyntaxErrors.size() <= pIndex )
	{
		return;
	}

	mSyntaxErrors[ pIndex ] = SE;
}

void SyntaxErrorPin::listSetSize( int pSize )
{
	Q_UNUSED( pSize )
}

int SyntaxErrorPin::sizeDimensions() const
{
	return( 1 );
}

float SyntaxErrorPin::size( int pDimension ) const
{
	return( pDimension == 0 ? mSyntaxErrors.size() : 0 );
}

float SyntaxErrorPin::sizeWidth() const
{
	return( mSyntaxErrors.size() );
}

float SyntaxErrorPin::sizeHeight() const
{
	return( 0 );
}

float SyntaxErrorPin::sizeDepth() const
{
	return( 0 );
}

QSizeF SyntaxErrorPin::toSizeF() const
{
	return( QSizeF( mSyntaxErrors.size(), 0 ) );
}

QVector3D SyntaxErrorPin::toVector3D() const
{
	return( QVector3D( mSyntaxErrors.size(), 0, 0 ) );
}
