#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QVector>

class AudioBuffer
{
public:
	AudioBuffer();
	~AudioBuffer();

	void clear( void );

public:
	qint64				 mAudPts;
	int					 mAudSmp;
	int					 mAudLen;
	QVector<quint8 *>	 mAudDat;
	bool				 mForce;
};

#endif // AUDIOBUFFER_H
