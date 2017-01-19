#ifndef MEDIARECORDFORM_H
#define MEDIARECORDFORM_H

#include <QWidget>

namespace Ui {
	class MediaRecorderForm;
}

class MediaPresetInterface;
class QProgressDialog;
class MediaRecorderNode;

class MediaRecorderForm : public QWidget
{
	Q_OBJECT

public:
	explicit MediaRecorderForm( MediaRecorderNode &pRecorder, QWidget *parent = 0);

	virtual ~MediaRecorderForm();

	QString presetName( void );

private slots:
	void on_mButtonRecord_clicked();

	void mediaPresetChanged(int index);

	void updatePreset( void );

	void recordingStarted( void );

	void recordingStopped( void );

	void recording( qreal pTimeStamp );

	void qualityUpdated(int position);

	void speedUpdated(int position);

	void videoFitUpdated(int index);

	void timeStartEdited( const QString pText );

	void timeDurationEdited( const QString pText );

	void timeEndEdited( const QString pText );

private:
	Ui::MediaRecorderForm		*ui;
	MediaPresetInterface		*mMediaPreset;
	QProgressDialog				*mProgressDialog;
	MediaRecorderNode			&VR;
};

#endif // MEDIARECORDFORM_H
