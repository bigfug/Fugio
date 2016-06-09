#include "audiofilterdisplay.h"

#include <QPainter>

#include <qmath.h>

AudioFilterDisplay::AudioFilterDisplay()
{
	mYR.resize( NP );
	mYI.resize( NP );
	mYM.resize( NP );

	mImage = QImage( NP, 64, QImage::Format_ARGB32 );

	{
		QPainter	P( &mImage );

		P.fillRect( mImage.rect(), Qt::black );
	}

	setPixmap( QPixmap::fromImage( mImage.scaled( NP, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) ) );
}

void AudioFilterDisplay::updateDisplay( const QVector<float> &pTaps )
{
	const int TapCnt = pTaps.size();
	const float	dw = float( M_PI ) / ( NP - 1.0f );

	float	mag_mag = -1;

	for( int i = 0 ; i < NP ; i++ )
	{
		const float	w = i * dw;

		mYR[i] = mYI[i] = 0;

		for( int k = 0 ; k < TapCnt ; k++ )
		{
			mYR[i] += pTaps[k] * qCos(k * w);
			mYI[i] -= pTaps[k] * qSin(k * w);
		}
	}

	for( int i = 0 ; i < NP ; i++ )
	{
		mYM[ i ] = qSqrt( mYR[ i ]  * mYR[ i ] + mYI[ i ] * mYI[ i ] );

		mag_mag = qMax( mag_mag, mYM[ i ] );
	}

	if( true )
	{
		QPainter	P( &mImage );

		P.fillRect( mImage.rect(), Qt::black );

		if( mag_mag > 0 )
		{
			P.setPen( Qt::white );

			for( int i = 0 ; i < NP ; i++ )
			{
				float	p = ( mYM[ i ] / mag_mag ) * 64.0f;

				P.drawLine( i, mImage.height() - 1, i, mImage.height() - 1 - p );
			}
		}
	}

	setPixmap( QPixmap::fromImage( mImage.scaled( NP, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) ) );
}
