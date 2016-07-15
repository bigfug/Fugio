#ifndef FLOATTHRESHOLDNODE_H
#define FLOATTHRESHOLDNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/paired_pins_helper_interface.h>

class FloatThresholdNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts floats into bools with a definable threshold" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Any_Trigger" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FloatThresholdNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FloatThresholdNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid(QSharedPointer<fugio::PinInterface> pPin) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputThreshold;
};

#endif // FLOATTHRESHOLDNODE_H
