#ifndef FUGIOUTIL_H
#define FUGIOUTIL_H

#include <QString>
#include <QStringList>
#include <QUuid>
#include <QPointF>
#include <QVariant>
#include <fugio/global.h>

#include <cmath>

FUGIO_NAMESPACE_BEGIN

class utils
{
public:
	static QString uuid2string( const QUuid &pUUID )
	{
		return( pUUID.toString().mid( 1, 36 ) );
	}

	static QUuid string2uuid( const QString &pString )
	{
		return( QUuid( "{" + pString + "}" ) );
	}

	static QString point2string( const QPointF &pInput )
	{
		return( QString( "%1,%2" ).arg( pInput.x() ).arg( pInput.y() ) );
	}

	static QPointF string2point( const QString &pString )
	{
		if( pString.startsWith( "@Variant" ) )
		{
			return( QVariant( pString ).toPointF() );
		}

		QStringList		L = pString.split( ',', QString::SkipEmptyParts );
		qreal			X = L.size() > 0 ? L.at( 0 ).toDouble() : 0.0;
		qreal			Y = L.size() > 1 ? L.at( 1 ).toDouble() : 0.0;

		return( QPointF( X, Y ) );
	}

	static QString timeToString( qreal pTimeStamp )
	{
		qreal			 TMSC;

		int				 Mils = ( modf( pTimeStamp, &TMSC ) * 1000.0 );
		int				 Mins = TMSC / 60.0;
		int				 Secs = fmod( TMSC, 60.0 );
		int				 Hour = Mins / 60;

		Mins %= 60;

		return( QString( "%1:%2:%3:%4" ).arg( Hour, int( 3 ), int( 10 ), QChar( '0' ) ).arg( Mins, int( 2 ), int( 10 ), QChar( '0' ) ).arg( Secs, int( 2 ), int( 10 ), QChar( '0' ) ).arg( Mils, int( 3 ), int( 10 ), QChar( '0' ) ) );
	}

	static qreal stringToTime( const QString &pString )
	{
		QStringList		l = pString.split( ':', QString::SkipEmptyParts );
		qreal			t = 0;

		if( !l.empty() )
		{
			t += qreal( l.takeLast().toLongLong() % 1000 ) / qreal( 1000 );
		}

		if( !l.empty() )
		{
			t += qreal( l.takeLast().toLongLong() % 60 );
		}

		if( !l.empty() )
		{
			t += qreal( l.takeLast().toLongLong() % 60 ) * qreal( 60 );
		}

		if( !l.empty() )
		{
			t += qreal( l.takeLast().toLongLong() ) * qreal( 60 * 60 );
		}

		return( t );
	}


	static int roundUp( int numToRound, int multiple )
	{
		if(multiple == 0)
		{
			return numToRound;
		}

		int remainder = numToRound % multiple;

		if (remainder == 0)
		{
			return numToRound;
		}

		return numToRound + multiple - remainder;
	}
};

FUGIO_NAMESPACE_END

#endif // FUGIOUTIL_H
