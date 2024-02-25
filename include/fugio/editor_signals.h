#ifndef EDITOR_SIGNALS_H
#define EDITOR_SIGNALS_H

#include <QObject>
#include <QUuid>

#include <fugio/global.h>

class ContextWidgetPrivate;

FUGIO_NAMESPACE_BEGIN
class EditInterface;
FUGIO_NAMESPACE_END

FUGIO_NAMESPACE_BEGIN

class EditorSignals : public QObject
{
	Q_OBJECT

signals:
	void editTargetChanged( fugio::EditInterface *pEditTarget );
};

FUGIO_NAMESPACE_END

#endif // EDITOR_SIGNALS_H
