#ifndef INSPECTOR_WIDGET_INTERFACE_H
#define INSPECTOR_WIDGET_INTERFACE_H

#include <QList>

class QWidget;

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class InspectorWidgetInterface
{
public:
	virtual ~InspectorWidgetInterface( void ) {}

	virtual QWidget *inspectorWidget( const QString &pValue ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::InspectorWidgetInterface, "com.bigfug.fugio.inspector-widget/1.0" )

#endif // INSPECTOR_WIDGET_INTERFACE_H
