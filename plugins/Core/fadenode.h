#ifndef FADENODE_H
#define FADENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class FadeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Smooths out a number over time" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Number_Smooth" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FadeNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~FadeNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

private slots:
	void onContextFrame( qint64 pTimeStamp );

	void pinAdded( QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface> );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinIncSpeed;
	QSharedPointer<fugio::PinInterface>			 mPinDecSpeed;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	qint64									 mLastTime;
	qreal									 mLastValue;
};

#endif // FADENODE_H
