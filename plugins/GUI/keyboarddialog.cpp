#include "keyboarddialog.h"
#include "ui_keyboarddialog.h"

KeyboardDialog::KeyboardDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::KeyboardDialog)
{
	ui->setupUi(this);
}

KeyboardDialog::~KeyboardDialog()
{
	delete ui;
}

QKeySequence KeyboardDialog::keyseq() const
{
	return( ui->mKeySeq->keySequence() );
}
