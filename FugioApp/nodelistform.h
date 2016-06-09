#ifndef NODELISTFORM_H
#define NODELISTFORM_H

#include <QWidget>
#include <QListWidgetItem>

#include <fugio/global_interface.h>

namespace Ui {
class NodeListForm;
}

class NodeListForm : public QWidget
{
	Q_OBJECT

public:
	explicit NodeListForm(QWidget *parent = 0);

	virtual ~NodeListForm();

public slots:
	void setNodelistUpdate( bool pEnabled );

private slots:
	void on_mNodeList_itemClicked(QListWidgetItem *item);

	void addNodeClass( const fugio::ClassEntry &pClassEntry );

	void on_mNodeList_itemSelectionChanged();

	void on_mNodeGroup_currentIndexChanged(const QString &arg1);

	void on_mNodeSearch_textChanged(const QString &arg1);

private:
	QStringList			  mNodeGroups;
	Ui::NodeListForm	 *ui;
};

#endif // NODELISTFORM_H
