#ifndef AUTORANGENODE_H
#define AUTORANGENODE_H

#include <QObject>
#include <QList>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class AutoRangeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Auto_Range" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit AutoRangeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AutoRangeNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;
	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected slots:
	void contextFrameStart( void );
	void contextFrameProcess( qint64 pTimeStamp );

private:
	void updateRange( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>		 mPinInput;

	QSharedPointer<fugio::PinInterface>		 mPinOutput;
	fugio::VariantInterface					*mValOutput;

	QSharedPointer<fugio::PinInterface>		 mPinMin;
	fugio::VariantInterface					*mValMin;

	QSharedPointer<fugio::PinInterface>		 mPinMax;
	fugio::VariantInterface					*mValMax;

	bool									 mValueAdded;
	double									 mCurVal;
	double									 mCurMax;
	double									 mCurMin;
	qint64									 mLastTime;
};

#endif // AUTORANGENODE_H
