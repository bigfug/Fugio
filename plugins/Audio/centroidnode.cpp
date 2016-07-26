#include "centroidnode.h"

#include <fugio/core/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/audio/fft_interface.h>

#include <qmath.h>

CentroidNode::CentroidNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mCentroid( 0 )
{
	mPinInputFFT = pinInput( "FFT" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Centroid", mPinOutput, PID_FLOAT );

	mPinInputFFT->setDescription( tr( "The frequency information calculated using a Fast Fourier Transform (FFT) node" ) );

	mPinOutput->setDescription( tr( "The centroid frequency - the most pronounced frequency in the FFT" ) );
}

void CentroidNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::FftInterface	*FFT = input<fugio::FftInterface *>( mPinInputFFT );

	if( !FFT || !FFT->fft() )
	{
		return;
	}

	const double	v = double( FFT->sampleRate() ) / double( FFT->samples() );

	double			u = 0;
	double			d = 0;

	for( int i = 0 ; i < FFT->samples() / 2 ; i++ )
	{
		double		x = FFT->fft()[ i * 2 ];

		x = sqrt( x * x );

		u += double( i ) * v * x;
		d += x;
	}

	if( d )
	{
		mCentroid = u / d;
	}
	else
	{
		mCentroid = 0;
	}

	if( mCentroid != mValOutput->variant().toFloat() )
	{
		mValOutput->setVariant( mCentroid );

		pinUpdated( mPinOutput );
	}
}
