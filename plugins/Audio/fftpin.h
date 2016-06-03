#ifndef FFTPIN_H
#define FFTPIN_H

#include <fugio/audio/uuid.h>
#include <fugio/pincontrolbase.h>
#include <fugio/audio/fft_interface.h>

class FFTPin : public fugio::PinControlBase, public fugio::FftInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::FftInterface )

public:
	Q_INVOKABLE explicit FFTPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~FFTPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const
	{
		return( QString() );
	}

	virtual QString description( void ) const
	{
		return( "FFT" );
	}

	//-------------------------------------------------------------------------
	// fugio::FftInterface

	virtual void setSource( fugio::FftInterface *pSource )
	{
		mFFT = pSource;
	}

	virtual qreal sampleRate( void ) const
	{
		return( mFFT ? mFFT->sampleRate() : 0 );
	}

	virtual int samples( void ) const
	{
		return( mFFT ? mFFT->samples() : 0 );
	}

	virtual int count( void ) const
	{
		return( mFFT ? mFFT->count() : 0 );
	}

	virtual const float *fft( void ) const
	{
		return( mFFT ? mFFT->fft() : 0 );
	}

private:
	fugio::FftInterface			*mFFT;
};

#endif // FFTPIN_H
