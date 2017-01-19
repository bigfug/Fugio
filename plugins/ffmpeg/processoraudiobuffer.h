#ifndef PROCESSOR_AUDIO_BUFFER_H
#define PROCESSOR_AUDIO_BUFFER_H

typedef struct ProcessorAudioBuffer
{
	QVector<QVector<float>>	 AudDat;
	QVector<QVector<float>>	 AudMix;
	QVector<QVector<float>>	 AudBuf;

	void clear( void )
	{
		for( QVector<float> &V : AudDat )
		{
			V.clear();
		}

		for( QVector<float> &V : AudMix )
		{
			V.clear();
		}

		for( QVector<float> &V : AudBuf )
		{
			V.clear();
		}

//		AudDat.clear();
//		AudMix.clear();
//		AudBuf.clear();
	}

	void setChannels( int c )
	{
		AudDat.resize( c );
		AudMix.resize( c );
		AudBuf.resize( c );
	}

	static inline float mix( float f1, float f2, float v )
	{
		return( ( f1 * v ) + ( f2 * ( 1.0f - v ) ) );
	}
} ProcessorAudioBuffer;

#endif // PROCESSOR_AUDIO_BUFFER_H

