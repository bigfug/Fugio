#ifndef CONTEXTSUBWINDOW_H
#define CONTEXTSUBWINDOW_H

#include <QMdiSubWindow>

class ContextWidgetPrivate;

class ContextSubWindow : public QMdiSubWindow
{
	Q_OBJECT

public:
	ContextSubWindow( QWidget *pParent = 0, Qt::WindowFlags pFlags = 0 );

	virtual ~ContextSubWindow();

	ContextWidgetPrivate *contextWidget( void );

protected:
	virtual void closeEvent( QCloseEvent * );

	// QWidget interface
protected:
	virtual void focusInEvent(QFocusEvent *);
	virtual void focusOutEvent(QFocusEvent *);
};

#endif // CONTEXTSUBWINDOW_H
