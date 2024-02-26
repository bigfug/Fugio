#ifndef INSPECTORFORM_H
#define INSPECTORFORM_H

#include <QWidget>

#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

namespace Ui {
class InspectorForm;
}

class InspectorForm : public QWidget
{
	Q_OBJECT

public:
	explicit InspectorForm(QWidget *parent = nullptr);
	~InspectorForm();

public slots:
	void inspectNode( QSharedPointer<fugio::ContextInterface> pContext, QUuid pNodeId );

private:
	Ui::InspectorForm *ui;
	QSharedPointer<fugio::ContextInterface>	mContext;
	QSharedPointer<fugio::NodeInterface>	mNode;
};

#endif // INSPECTORFORM_H
