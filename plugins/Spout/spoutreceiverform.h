#ifndef SPOUTRECEIVERFORM_H
#define SPOUTRECEIVERFORM_H

#include <QWidget>

namespace Ui {
class SpoutReceiverForm;
}

class SpoutReceiverForm : public QWidget
{
	Q_OBJECT

public:
	explicit SpoutReceiverForm(QWidget *parent = 0);

	virtual ~SpoutReceiverForm();

public slots:
	void setReceiverName( const QString &pName );

	void setReceiverList( const QStringList &pNameList );

signals:
	void receiverName( const QString &pName );

private slots:
	void on_mName_currentTextChanged(const QString &arg1);

private:
	Ui::SpoutReceiverForm *ui;

	QStringList				 mNameList;
};

#endif // SPOUTRECEIVERFORM_H
