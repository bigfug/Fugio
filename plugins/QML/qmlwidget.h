#ifndef QMLWIDGET_H
#define QMLWIDGET_H

#include <QQuickWidget>

class QMLWidget : public QQuickWidget
{
	Q_OBJECT

public:
	explicit QMLWidget();

	virtual ~QMLWidget( void ) {}

public slots:
	void updateNodeName( const QString &pName );
};

#endif // QMLWIDGET_H
