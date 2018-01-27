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
	FUGID( PIN_INPUT_FFT,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_COUNT,		"071d3ea3-7236-4bd1-bb50-a9e25eb93ed9" );
	FUGID( PIN_OUTPUT_BANDS,	"6e978827-5f19-4f75-be8b-5f7a3d00595b" );

	mPinInputFFT = pinInput( "FFT", PIN_INPUT_FFT );

	mPinInputFFT->registerPinInputType( PID_FFT );

	mPinInputCount = pinInput( "Bands", PIN_INPUT_COUNT );

	mPinInputCount->registerPinInputType( PID_INTEGER );

	mPinInputCount->setValue( 12 );

	mValOutputBands = pinOutput<fugio::VariantInterface *>( "Bands", mPinOutputBands, PID_FLOAT, PIN_OUTPUT_BANDS );

	mValOutputBands->setVariantCount( 12 );

	mPinInputFFT->setDescription( tr( "The frequency information calculated using a Fast Fourier Transform (FFT) node" ) );
}

float FrequencyBandsNode::getBandWidth( float timeSize, float sampleRate )
{
	return( ( 2.0f / timeSize ) * ( sampleRate / 2.0f ) );
}

int FrequencyBandsNode::freqToIndex( int timeSize, int sampleRate, int freq)
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
	const int			BandCount = variant( mPinInputCount ).toInt();

	if( BandCount <= 0 )
	{
		return;
	}

	if( BandCount != mValOutputBands->variantCount() )
	{
		mValOutputBands->setVariantCount( BandCount );
	}

	if( mPinInputFFT->isUpdated( pTimeStamp ) )
	{
		fugio::FftInterface		*FFT = input<fugio::FftInterface *>( mPinInputFFT );

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

		for( int i = 0 ; i < BandCount ; i++ )
		{
			float	avg = 0;
			int		lowFreq = ( !i ? 0 : (int)((sampleRate/2) / powf( 2, BandCount - i ) ) );
			int		hiFreq  = (int)((sampleRate/2) / powf( 2, BandCount - 1 - i));
			int		lowBound = freqToIndex( timeSize, sampleRate, lowFreq );
			int		hiBound = freqToIndex( timeSize, sampleRate, hiFreq );

			for (int j = lowBound; j <= hiBound; j++)
			{
				const float	re = ( DAT[ ( i * 2 ) + 0 ] * 2.0f ) / float( timeSize );
				const float im = ( DAT[ ( i * 2 ) + 1 ] * 2.0f ) / float( timeSize );

				avg += sqrtf( re * re + im * im ) * 10.0f;
			}

			// line has been changed since discussion in the comments
			// avg /= (hiBound - lowBound);
			avg /= (hiBound - lowBound + 1);

			mValOutputBands->setVariant( i, avg );
		}

		pinUpdated( mPinOutputBands );
	}
}

