#ifndef INTERVALNODE_H
#define INTERVALNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/paired_pins_helper_interface.h>

class IntervalNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Midi_Interval" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE IntervalNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~IntervalNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface

public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;


protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputInterval;
	QSharedPointer<fugio::PinInterface>			 mPinInputNote;

	fugio::ChoiceInterface						*mValInputInterval;

	QSharedPointer<fugio::PinInterface>			 mPinOutputNote;
	fugio::VariantInterface						*mValOutputNote;

	static QMap<QString,int>					 mIntervalMap;
};

#endif // INTERVALNODE_H
