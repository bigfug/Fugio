#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include <fugio/editor_interface.h>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog( QWidget *parent = 0 );

	virtual ~SettingsDialog();

private slots:
	void on_mSnippetsSelect_clicked();

	void dialogAccepted();

private:
	Ui::SettingsDialog *ui;

	QMap<fugio::SettingsInterface *, QWidget *>		mSettings;
};

#endif // SETTINGSDIALOG_H
