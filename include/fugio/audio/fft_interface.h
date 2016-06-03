#ifndef INTERFACE_FFT_H
#define INTERFACE_FFT_H

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class FftInterface
{
public:
	virtual ~FftInterface( void ) {}

	virtual void setSource( FftInterface *pSource ) = 0;

	virtual qreal sampleRate( void ) const = 0;
	virtual int samples( void ) const = 0;
	virtual int count( void ) const = 0;
	virtual const float *fft( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::FftInterface, "com.bigfug.fugio.fft/1.0" )

#endif // INTERFACE_FFT_H

