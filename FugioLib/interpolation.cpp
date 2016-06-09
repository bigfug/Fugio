
#include <fugio/interpolation.h>

#include <QObject>
#include <QMap>
#include <QPair>

#include <qmath.h>

FUGIO_NAMESPACE_BEGIN

typedef QMap<Interpolate,QPair<QString,QString> >	InterpolateDataMap;

InterpolateDataMap		InterpolateData;

void initInterpolateData( void )
{
	if( InterpolateData.isEmpty() )
	{
		InterpolateData.insert( DEFAULT, QPair<QString,QString>( "default",QObject::tr( "Default" ) ) );
		InterpolateData.insert( LINEAR, QPair<QString,QString>( "linear",QObject::tr( "Linear" ) ) );
		InterpolateData.insert( COSINE, QPair<QString,QString>( "cosine",QObject::tr( "Cosine" ) ) );
		InterpolateData.insert( LINEAR_FALL, QPair<QString,QString>( "linear-fall",QObject::tr( "Linear Fall" ) ) );
		InterpolateData.insert( LINEAR_RISE, QPair<QString,QString>( "linear-rise",QObject::tr( "Linear Rise" ) ) );
		InterpolateData.insert( FORWARD, QPair<QString,QString>( "time-forward", QObject::tr( "Time Forward" ) ) );
		InterpolateData.insert( BACKWARD, QPair<QString,QString>( "time-backward", QObject::tr( "Time Backward" ) ) );
		InterpolateData.insert( HOLD, QPair<QString,QString>( "hold", QObject::tr( "Hold" ) ) );
		//InterpolateData.insert( LOG_FALL, QPair<QString,QString>( "log-fall",QObject::tr( "Log Fall" ) ) );
		InterpolateData.insert( GAMMA_22, QPair<QString,QString>( "gamma-22", QObject::tr( "Gamma (2.2)" ) ) );
		InterpolateData.insert( GAMMA_CORRECTION_22, QPair<QString,QString>( "gamma-correction-22", QObject::tr( "Gamma Correction (2.2)" ) ) );
	}
}

QStringList defaultInterpolateNames()
{
	QStringList		Names;

	initInterpolateData();

	for( InterpolateDataMap::const_iterator it = InterpolateData.begin() ; it != InterpolateData.end() ; it++ )
	{
		if( it.key() != DEFAULT )
		{
			Names.append( it.value().second );
		}
	}

	return( Names );
}

QStringList keyframeInterpolateNames()
{
	QStringList		Names;

	initInterpolateData();

	for( InterpolateDataMap::const_iterator it = InterpolateData.begin() ; it != InterpolateData.end() ; it++ )
	{
		Names.append( it.value().second );
	}

	return( Names );
}

Interpolate interpolateFromName( const QString &pString )
{
	initInterpolateData();

	for( InterpolateDataMap::const_iterator it = InterpolateData.begin() ; it != InterpolateData.end() ; it++ )
	{
		if( pString.compare( it.value().first ) == 0 )
		{
			return( it.key() );
		}
	}

	return( DEFAULT );
}

Interpolate interpolateFromHumanName(const QString &pString)
{
	initInterpolateData();

	for( InterpolateDataMap::const_iterator it = InterpolateData.begin() ; it != InterpolateData.end() ; it++ )
	{
		if( pString.compare( it.value().second ) == 0 )
		{
			return( it.key() );
		}
	}

	return( LINEAR );
}

QString interpolateToName( Interpolate pInterpolate )
{
	initInterpolateData();

	InterpolateDataMap::const_iterator it = InterpolateData.find( pInterpolate );

	return( it == InterpolateData.end() ? "invalid" : it.value().first );
}

QString interpolateToHumanName( Interpolate pInterpolate )
{
	initInterpolateData();

	InterpolateDataMap::const_iterator it = InterpolateData.find( pInterpolate );

	return( it == InterpolateData.end() ? "Invalid" : it.value().second );
}

qreal interpolate( Interpolate ki, qreal v1, qreal v2, qreal t1, qreal t2, qreal ts, qreal tp, qreal dur )
{
	Q_UNUSED( t2 )

	qreal		v = v1;

	switch( ki )
	{
		case DEFAULT:
		case LINEAR:
			{
				const qreal		vr = v2 - v1;

				v = v1 + vr * tp;
			}
			break;

		case COSINE:
			{
				const qreal		mu = ( 1.0 - qCos( tp * M_PI ) ) / 2.0;

				v = v1 * ( 1.0 - mu ) + v2 * mu;
			}
			break;

		case LINEAR_FALL:
			v = ( 1.0 - tp ) * v1;
			break;

		case LINEAR_RISE:
			v = tp * v2;
			break;

		case LOG_FALL:
			v = log10( 1.0 + ( ( 1.0 - tp ) * 9.0 ) ) * v1;
			break;

		case HOLD:
			v = v1;
			break;

		case FORWARD:
			v = v1 + ( qreal( ts - t1 ) / dur );
			break;

		case BACKWARD:
			v = v1 - ( qreal( ts - t1 ) / dur );
			break;

		case GAMMA_22:
			{
				const qreal		mu = qPow( tp, 2.2 );

				v = v1 * ( 1.0 - mu ) + v2 * mu;
			}
			break;

		case GAMMA_CORRECTION_22:
			{
				const qreal		mu = qPow( tp, 1.0 / 2.2 );

				v = v1 * ( 1.0 - mu ) + v2 * mu;
			}
			break;
	}

	return( v );
}

FUGIO_NAMESPACE_END
