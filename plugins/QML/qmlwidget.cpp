#include "qmlwidget.h"

#include <QDockWidget>

QMLWidget::QMLWidget()
{
	// Workaround for QTBUG-62443

#if defined( Q_OS_WIN )
	quickWindow()->setPersistentSceneGraph( false );
#endif
}

void QMLWidget::updateNodeName(const QString &pName)
{
	QDockWidget		*W = qobject_cast<QDockWidget *>( parent() );

	if( W )
	{
		W->setWindowTitle( QString( "QML: %1" ).arg( pName ) );
	}
}
