#include "frequencybandsnode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/audio/fft_interface.h>

#include <fugio/context_signals.h>

#include <qmath.h>

#define MAG_SMP	(48000/50)

// http://code.compartmental.net/2007/03/21/fft-averages/

FrequencyBandsNode::FrequencyBandsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinFFT = pinInput( "FFT" );

	for( int i = 0 ; i < 12 ; i++ )
	{
		int		lowFreq = ( !i ? 0 : (int)((48000/2) / (float)qPow(2, 12 - i)) );
		int		hiFreq  = (int)((48000/2) / (float)qPow(2, 11 - i));

		QSharedPointer<fugio::PinInterface>	 PIN;
		fugio::VariantInterface				*VAL = pinOutput<fugio::VariantInterface *>( QString( "Band %1" ).arg( i ), PIN, PID_FLOAT );

		if( PIN && VAL )
		{
			mPinOutput.append( PIN );

			mValOutput.append( VAL );

			PIN->setDescription( tr( "The power of frequencies between %1Hz and %2Hz" ).arg( lowFreq ).arg( hiFreq ) );
		}
	}

	mPinFFT->setDescription( tr( "The frequency information calculated using a Fast Fourier Transform (FFT) node" ) );

}

float getBandWidth( float timeSize, float sampleRate )
{
	return( ( 2.0f / timeSize ) * ( sampleRate / 2.0f ) );
}

int freqToIndex( int timeSize, int sampleRate, int freq)
{
  // special case: freq is lower than the bandwidth of spectrum[0]
  if ( freq < getBandWidth( timeSize, sampleRate )/2 ) return 0;
  // special case: freq is within the bandwidth of spectrum[512]
  if ( freq > sampleRate/2 - getBandWidth( timeSize, sampleRate )/2 ) return timeSize/2;
  // all other cases
  float fraction = (float)freq/(float) sampleRate;
  int i = qRound(timeSize * fraction);
  return i;
}

void FrequencyBandsNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !pTimeStamp || mPinFFT->isUpdated( pTimeStamp ) )
	{
		fugio::FftInterface		*FFT = input<fugio::FftInterface *>( mPinFFT );

		if( !FFT )
		{
			return;
		}

		int			sampleRate = FFT->sampleRate();
		int			timeSize   = FFT->samples();

		const float				*DAT = FFT->fft();

		if( !DAT )
		{
			return;
		}

		for( int i = 0 ; i < 12 ; i++ )
		{
			float	avg = 0;
			int		lowFreq = ( !i ? 0 : (int)((sampleRate/2) / (float)qPow(2, 12 - i)) );
			int		hiFreq  = (int)((sampleRate/2) / (float)qPow(2, 11 - i));
			int		lowBound = freqToIndex(timeSize, sampleRate, lowFreq);
			int		hiBound = freqToIndex(timeSize, sampleRate, hiFreq);

			for (int j = lowBound; j <= hiBound; j++)
			{
				const float	re = ( DAT[ ( i * 2 ) + 0 ] * 2.0f ) / float( timeSize );
				const float im = ( DAT[ ( i * 2 ) + 1 ] * 2.0f ) / float( timeSize );

				avg += sqrtf( re * re + im * im ) * 10.0f;
			}

			// line has been changed since discussion in the comments
			// avg /= (hiBound - lowBound);
			avg /= (hiBound - lowBound + 1);

			mValOutput[ i ]->setVariant( avg );

			pinUpdated( mPinOutput[ i ] );
		}
	}
}
