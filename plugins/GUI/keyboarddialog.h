#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include <QDialog>

namespace Ui {
	class KeyboardDialog;
}

class KeyboardDialog : public QDialog
{
	Q_OBJECT

public:
	explicit KeyboardDialog(QWidget *parent = 0);
	~KeyboardDialog();

	QKeySequence keyseq( void ) const;

private:
	Ui::KeyboardDialog *ui;
};

#endif // KEYBOARDDIALOG_H
