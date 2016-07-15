#ifndef SIGNALNODE_H
#define SIGNALNODE_H

#include <QObject>

#include <qmath.h>
#include <ctime>

#include <fugio/nodecontrolbase.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/audio/audio_instance_base.h>

class SignalNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Simple signal generator" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Signal_(Audio)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SignalNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~SignalNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	// InterfaceAudioProducer interface
public:
	virtual fugio::AudioInstanceBase *audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	//virtual void audioFreeInstance(void *pInstanceData) Q_DECL_OVERRIDE;
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pInstance )

		return( true );
	}

private slots:
	void onContextFrame( qint64 pTimeStamp );

protected:
	//  Author: Andrew Simper of Vellocet
	//          andy@vellocet.com

	class VRand
	{
	public:
	  enum
	  {
		NumPinkBins  = 16,
		NumPinkBins1 = NumPinkBins-1
	  };

	  VRand()
	  {
		m_count = 1;
		m_brown = 0.0f;
		m_pink  = 0;
		for (int i=0; i<NumPinkBins; i++)
		{
		  m_pinkStore[i] = 0.0f;
		}
	  }

	  void seed(unsigned long seed=0)
	  {
		if (seed == 0) qsrand( time(NULL) );
		else           qsrand( seed );
	  };

	  // returns psuedo random white noise number
	  // in the range -scale to scale
	  //
	  inline float white(float scale=0.5f)
	  {
		  return( ( 1.0f - ( float( qrand() ) / float( RAND_MAX / 2 ) ) ) * scale );
	  }


	#if defined( WIN32 ) && !defined( WIN64 )
	  int inline CTZ(int num)
	  {
		_asm
		{
		  mov ecx, num
		  bsf eax, ecx
		  mov num, eax
		}
		return num;
	  }
	#else // please contribute assembler for mac/sparc/sgi/etc.
	  int inline CTZ(int num)
	  {
		unsigned int i=0;
		while (((num>>i)&1)==0 && i<sizeof(int)) i++;
		return i;
	  }
	#endif

	  // returns pink noise random number in the range -0.5 to 0.5
	  //
	  inline float pink(void)
	  {
		float prevr;
		float r;
		unsigned long k;
		k = CTZ(m_count);
		k = k & NumPinkBins1;

		// get previous value of this octave
		prevr = m_pinkStore[k];

		while (true)
		{
		  r = white();

		  // store new value
		  m_pinkStore[k] = r;

		  r -= prevr;

		  // update total
		  m_pink += r;

		  if (m_pink <-4.0f || m_pink > 4.0f) m_pink -= r;
		  else break;
		}

		// update counter
		m_count++;

		return (white() + m_pink)*0.125f;
	  }

	  // returns brown noise random number in the range -0.5 to 0.5
	  //
	  inline float brown(void)
	  {
		while (true)
		{
		  float  r = white();
		  m_brown += r;
		  if (m_brown<-8.0f || m_brown>8.0f) m_brown -= r;
		  else break;
		}
		return m_brown*0.0625f;
	  }

	private:
	  //unsigned long  m_seed;
	  unsigned long  m_count;
//	  unsigned long  m_white;
	  float          m_pink;
	  float          m_brown;
	  float          m_pinkStore[NumPinkBins];
	};

	static inline float sine( const float pPhase )
	{
		return( qSin( pPhase * 2.0 * M_PI ) );
	}

	inline float offset( const float pPhase ) const
	{
		return( pPhase > mPhaseCenter ? 0.5f + ( 0.5f * ( ( pPhase - mPhaseCenter ) / ( 1.0f - mPhaseCenter ) ) ) : 0.5f * ( pPhase / mPhaseCenter ) );
	}

protected:
	typedef enum SignalType
	{
		SINE, SQUARE, SAW, TRIANGLE, WHITE, PINK, BROWN
	} SignalType;

	class AudioInstanceData : public fugio::AudioInstanceBase
	{
	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{

		}

		virtual ~AudioInstanceData( void ) {}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers ) Q_DECL_OVERRIDE
		{
			QSharedPointer<SignalNode>	API = qSharedPointerCast<SignalNode>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

	public:
		QVector<float>					mSmpBuf;
		qint64							mSamplePosition;
		double							mPhase;
		VRand							mVRand;

		double							phase_index;
		double							phase_delta;
	};

	void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;

	void generateSignal( qint64 pSamplePostion, qint64 pSampleCount, const float SmpCnt, float *pBuffer, float pVolume, AudioInstanceData &AID ) const;

	static QMap<QString,SignalType>			 mSignalTypes;

	QSharedPointer<fugio::PinInterface>		 mPinSignalType;
	QSharedPointer<fugio::PinInterface>		 mPinInputFrequency;
	QSharedPointer<fugio::PinInterface>		 mPinInputVolume;
	QSharedPointer<fugio::PinInterface>		 mPinInputPhaseCenter;
	QSharedPointer<fugio::PinInterface>		 mPinInputPhaseOffset;
	QSharedPointer<fugio::PinInterface>		 mPinInputBias;

	QSharedPointer<fugio::PinInterface>		 mPinOutput;
	fugio::AudioProducerInterface			*mValOutput;

	fugio::ChoiceInterface					*mPinSignalTypeChoice;

	SignalType								 mSignalType;
	volatile double							 mFrequency;
	volatile double							 mVolume;
	double									 mPhaseCenter;
	double									 mPhaseOffset;
	double									 mBias;
};

#endif // SIGNALNODE_H
