#include "spoutreceiverform.h"
#include "ui_spoutreceiverform.h"

SpoutReceiverForm::SpoutReceiverForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SpoutReceiverForm)
{
	ui->setupUi(this);
}

SpoutReceiverForm::~SpoutReceiverForm()
{
	delete ui;
}

void SpoutReceiverForm::setReceiverName(const QString &pName)
{
	ui->mName->setCurrentText( pName );
}

void SpoutReceiverForm::setReceiverList( const QStringList &pNameList )
{
	ui->mName->clear();

	for( QString S : pNameList )
	{
		ui->mName->addItem( S );
	}

	mNameList = pNameList;
}

void SpoutReceiverForm::on_mName_currentTextChanged(const QString &arg1)
{
	emit receiverName( arg1 );
}
