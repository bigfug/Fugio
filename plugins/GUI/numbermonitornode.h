#ifndef NUMBERMONITORNODE_H
#define NUMBERMONITORNODE_H

#include <QObject>

#include <QDockWidget>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class NumberMonitorForm;

class NumberMonitorNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Graphs the value of a number over time." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Number_Monitor" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit NumberMonitorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NumberMonitorNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void );

	virtual bool initialise( void );

	virtual bool deinitialise( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

protected slots:
	void onShowClicked( void );

	void onContextFrame( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinValue;

	QDockWidget								*mDockWidget;
	NumberMonitorForm						*mMonitor;

	Qt::DockWidgetArea						 mDockArea;

	qint64									 mLastTime;
};

#endif // NUMBERMONITORNODE_H
