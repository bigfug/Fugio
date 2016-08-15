#ifndef DATENODE_H
#define DATENODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class DateNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Date" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Date" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit DateNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DateNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

private slots:
	void contextFrameStart( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinOutputDay;
	fugio::VariantInterface						*mValOutputDay;

	QSharedPointer<fugio::PinInterface>			 mPinOutputDayOfWeek;
	fugio::VariantInterface						*mValOutputDayOfWeek;

	QSharedPointer<fugio::PinInterface>			 mPinOutputDayOfYear;
	fugio::VariantInterface						*mValOutputDayOfYear;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMonth;
	fugio::VariantInterface						*mValOutputMonth;

	QSharedPointer<fugio::PinInterface>			 mPinOutputYear;
	fugio::VariantInterface						*mValOutputYear;

	QSharedPointer<fugio::PinInterface>			 mPinOutputWeekNumber;
	fugio::VariantInterface						*mValOutputWeekNumber;
};

#endif // DATENODE_H
