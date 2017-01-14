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
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Number_Monitor_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit NumberMonitorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NumberMonitorNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

protected slots:
	void onShowClicked( void );

	void updateNodeName( const QString &pName );

signals:
	void nodeNameUpdated( const QString &pName );

private:
	QSharedPointer<fugio::PinInterface>		 mPinInputTrigger;

	QSharedPointer<fugio::PinInterface>		 mPinInputValue;

	QDockWidget								*mDockWidget;
	NumberMonitorForm						*mMonitor;

	Qt::DockWidgetArea						 mDockArea;
};

#endif // NUMBERMONITORNODE_H
