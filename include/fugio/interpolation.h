#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <QStringList>

#include "global.h"

FUGIO_NAMESPACE_BEGIN

typedef enum Interpolate
{
	DEFAULT,
	LINEAR,
	COSINE,
	LINEAR_FALL,
	LINEAR_RISE,
	LOG_FALL,
	HOLD,
	FORWARD,
	BACKWARD,
	GAMMA_22,
	GAMMA_CORRECTION_22
} Interpolate;

QStringList defaultInterpolateNames( void );
QStringList keyframeInterpolateNames( void );
Interpolate interpolateFromName( const QString &pString );
Interpolate interpolateFromHumanName( const QString &pString );
QString interpolateToName( Interpolate pInterpolate );
QString interpolateToHumanName( Interpolate pInterpolate );

qreal interpolate( Interpolate ki, qreal v1, qreal v2, qreal t1, qreal t2, qreal ts, qreal tp, qreal dur );

FUGIO_NAMESPACE_END

#endif // INTERPOLATION_H

