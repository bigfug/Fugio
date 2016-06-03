#ifndef NODEEDITORWIZARD_H
#define NODEEDITORWIZARD_H

#include <QWizard>

class NodeEditorWizard : public QWizard
{
	Q_OBJECT

public:
	explicit NodeEditorWizard( QWidget *pParent = 0 );

	virtual ~NodeEditorWizard();

	static int version( void )
	{
		return( 1 );
	}

signals:

public slots:
};

#endif // NODEEDITORWIZARD_H
