#ifndef INTERFACE_CONTEXT_WIDGET_H
#define INTERFACE_CONTEXT_WIDGET_H

#include <QUuid>

#include "global.h"

class QUndoStack;
class QWidget;

FUGIO_NAMESPACE_BEGIN

#define IID_CONTEXT_WIDGET		(QUuid("{EFED605F-F5F2-4da5-9B7A-8E03148C365C}"))

class ContextWidgetSignals;

class ContextWidgetInterface
{
public:
	virtual ~ContextWidgetInterface( void ) {}

	virtual void addWidget( QWidget *pWidget ) = 0;

	virtual QUndoStack *undoStack( void ) = 0;

	virtual fugio::ContextWidgetSignals *qobject( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ContextWidgetInterface, "com.bigfug.fugio.context.widget/1.0" )

#endif // INTERFACE_CONTEXT_WIDGET_H
