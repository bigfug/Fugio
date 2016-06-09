#ifndef CONTEXTWIDGET_H
#define CONTEXTWIDGET_H

#include <QObject>
#include <QUuid>

#include <fugio/global.h>
#include <fugio/context_widget_interface.h>

class ContextWidgetPrivate;
class QUndoStack;

FUGIO_NAMESPACE_BEGIN

class ContextWidgetSignals : public QObject
{
	Q_OBJECT

    friend class ::ContextWidgetPrivate;

signals:
	void updateGui( void );

protected:
	ContextWidgetSignals( ContextWidgetPrivate *p );

	virtual ~ContextWidgetSignals( void );

private:
	ContextWidgetPrivate	*mPrivate;
};

FUGIO_NAMESPACE_END

#endif // CONTEXTWIDGET_H
