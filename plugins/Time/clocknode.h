#ifndef CLOCKNODE_H
#define CLOCKNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class ClockNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Breaks down the current (or specified) time into various outputs." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Clock" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ClockNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ClockNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual bool initialise( void );

	virtual bool deinitialise( void );

private slots:
	void frameStart( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputTime;

	QSharedPointer<fugio::PinInterface>			 mPinOutputHour24;
	fugio::VariantInterface						*mValOutputHour24;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMinute;
	fugio::VariantInterface						*mValOutputMinute;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSecond;
	fugio::VariantInterface						*mValOutputSecond;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMillis;
	fugio::VariantInterface						*mValOutputMillis;

	QSharedPointer<fugio::PinInterface>			 mPinOutputDayMillis;
	fugio::VariantInterface						*mValOutputDayMillis;

	QSharedPointer<fugio::PinInterface>			 mPinOutputDayPosition;
	fugio::VariantInterface						*mValOutputDayPosition;

	QSharedPointer<fugio::PinInterface>			 mPinOutputHourPosition;
	fugio::VariantInterface						*mValOutputHourPosition;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMinutePosition;
	fugio::VariantInterface						*mValOutputMinutePosition;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSecondPosition;
	fugio::VariantInterface						*mValOutputSecondPosition;
};

#endif // CLOCKNODE_H
