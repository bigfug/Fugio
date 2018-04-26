#ifndef NODENAMEDIALOG_H
#define NODENAMEDIALOG_H

#include <QDialog>
#include <QUuid>
#include <QListWidgetItem>

namespace Ui {
class NodeNameDialog;
}

class NodeNameDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NodeNameDialog(QWidget *parent = 0);

	virtual ~NodeNameDialog();

	QUuid nodename( void ) const
	{
		return( mNodeUuid );
	}

private slots:
	void on_lineEdit_textChanged(const QString &arg1);

	void on_listWidget_doubleClicked(const QModelIndex &index);

	void on_lineEdit_returnPressed();

	void on_listWidget_itemActivated(QListWidgetItem *item);

protected:
	void setListHistory( void );
	void setListCompare( const QString &pCmp );

	void addToHistory( const QUuid &pNodeUuid );

	QListWidgetItem *fillWidgetItem( const QString &pName, const QUuid &pUuid );

private:
	Ui::NodeNameDialog		*ui;

	QList<QUuid>			 mNodeList;
	QUuid					 mNodeUuid;
};

#endif // NODENAMEDIALOG_H
