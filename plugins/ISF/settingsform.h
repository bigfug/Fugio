#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QWidget
{
	Q_OBJECT

public:
	explicit SettingsForm(QWidget *parent = 0);
	~SettingsForm();

	QString path( void ) const
	{
		return( mPath );
	}

public slots:
	void setPath( QString pPath )
	{
		if( pPath != mPath )
		{
			mPath = pPath;

			emit pathChanged( mPath );
		}
	}

signals:
	void pathChanged( QString pPath );

private slots:
	void on_mISFButton_clicked();

private:
	Ui::SettingsForm *ui;

	QString				 mPath;
};

#endif // SETTINGSFORM_H
