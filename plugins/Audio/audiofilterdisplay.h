#ifndef AUDIOFILTERDISPLAY_H
#define AUDIOFILTERDISPLAY_H

#include <QLabel>
#include <QVector>
#include <QPixmap>
#include <QImage>

class AudioFilterDisplay : public QLabel
{
	Q_OBJECT

public:
	AudioFilterDisplay();

	virtual ~AudioFilterDisplay( void ) {}

public slots:
	void updateDisplay( const QVector<float> &pTaps );

private:
	static const int	NP = 128;

	QVector<float>	mYR;
	QVector<float>	mYI;
	QVector<float>	mYM;

	QImage			mImage;
};

#endif // AUDIOFILTERDISPLAY_H
