#ifndef FFTNODE_H
#define FFTNODE_H

#include <QObject>
#include <QLabel>

#include <qmath.h>

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_instance_base.h>
#include <fugio/audio/fft_interface.h>
#include <fugio/choice_interface.h>

#if defined( FFTW_PLUGIN_SUPPORTED )
#include <fftw3.h>
#endif

class FFTNode : public fugio::NodeControlBase, public fugio::FftInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::FftInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Fast Fourier Transform." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/FFT" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FFTNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FFTNode( void );

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// fugio::FftInterface interface
public:
	virtual qreal sampleRate() const Q_DECL_OVERRIDE
	{
		return( 48000.0 );
	}

	virtual void setSource( fugio::FftInterface * ) Q_DECL_OVERRIDE {}

	virtual inline int samples() const Q_DECL_OVERRIDE
	{
		return( mSampleCount );
	}

	virtual inline int shift() const Q_DECL_OVERRIDE
	{
		return( mSampleShift );
	}

	virtual int count() const Q_DECL_OVERRIDE
	{
		return( samples() / 2 + 1 );
	}

	virtual const float *fft() const Q_DECL_OVERRIDE
	{
#if defined( FFTW_PLUGIN_SUPPORTED )
		return( (const float *)mBufDst );
#else
		return( nullptr );
#endif
	}

protected slots:
	void onContextFrame( qint64 pTimeStamp );

protected:
	void processAudio( qint64 pTimeStamp );

	void calculateWindow();

	// FFT Window Functions
	// nn = samples() - 1

	inline static float hanning( float i, float nn )
	{
		return( 0.5f * ( 1.0f - cosf( 2.0f * M_PI * i / nn ) ) );
	}

	inline static float hamming( float i, float nn )
	{
		return( 0.54f - 0.46f * cosf( 2.0f * M_PI * i / nn ) );
	}

	inline static float blackman( float i, float nn )
	{
		return( 0.42f - 0.5f * cosf( 2.0f * M_PI * i / nn )
					 + 0.08f * cosf( 4.0f * M_PI * i / nn ) );
	}

	inline static float parzen( float i, float nn )
	{
		return( 1.0f - fabsf( ( i - 0.5f * nn ) / ( 0.5f *( nn + 2.0f ) ) ) );
	}

	inline static float welch( float i, float nn )
	{
		float	t = ( i - 0.5f * nn ) / ( 0.5f * ( nn + 2.0f ) );

		return( 1.0f - ( t * t ) );
	}

	inline static float steeper( float i, float nn )
	{
		return ( 0.375f
				 - 0.5f   * cosf( 2.0f * M_PI * i / nn )
				 + 0.125f * cosf( 4.0f * M_PI * i / nn ) );
	}

protected:
	typedef enum WindowType
	{
		NONE, HANNING, HAMMING, BLACKMAN, PARZEN, WELCH, STEEPER
	} WindowType;

	static QMap<QString,FFTNode::WindowType> mWindowTypes;

	QSharedPointer<fugio::PinInterface>		 mPinInputAudio;
	QSharedPointer<fugio::PinInterface>		 mPinInputSamples;
	QSharedPointer<fugio::PinInterface>		 mPinInputShift;
	QSharedPointer<fugio::PinInterface>		 mPinInputWindow;

	QSharedPointer<fugio::PinInterface>		 mPinOutputFFT;
	fugio::FftInterface						*mValOutputFFT;

	fugio::ChoiceInterface					*mPinInputWindowChoice;

	WindowType								 mWindowType;
	int										 mSampleCount;
	qint64									 mSamplePosition;
	qint64									 mSampleShift;

	fugio::AudioInstanceBase				*mProducerInstance;

	QVector<float>							 mWindow;

#if defined( FFTW_PLUGIN_SUPPORTED )
	fftwf_plan								 mPlan;
	float									*mBufSrc;
	fftwf_complex							*mBufDst;
#endif
	void fftwFree();
};

#endif // FFTNODE_H
