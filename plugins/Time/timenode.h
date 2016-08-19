#ifndef TIMENODE_H
#define TIMENODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class TimeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Time" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Time" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TimeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TimeNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

private slots:
	void contextFrameStart( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputInterval;

	QSharedPointer<fugio::PinInterface>			 mPinOutputContextTimeMs;
	fugio::VariantInterface						*mValOutputContextTimeMs;

	QSharedPointer<fugio::PinInterface>			 mPinOutputContextTimeS;
	fugio::VariantInterface						*mValOutputContextTimeS;

	qint64										 mLastUpdate;
};

#endif // TIMENODE_H
