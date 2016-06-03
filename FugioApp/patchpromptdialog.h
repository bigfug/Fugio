#ifndef PATCHPROMPTDIALOG_H
#define PATCHPROMPTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class PatchPromptDialog;
}

class PatchPromptDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PatchPromptDialog(QWidget *parent = 0);

	virtual ~PatchPromptDialog();

private slots:
	void on_mButtonNewPatch_clicked();

	void on_mButtonOpenPatch_clicked();

	void on_mButtonCancel_clicked();

	void on_mRecentPatches_itemDoubleClicked( QListWidgetItem *item );

private:
	Ui::PatchPromptDialog *ui;
};

#endif // PATCHPROMPTDIALOG_H
