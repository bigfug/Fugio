#ifndef SUBTRACTNODE_H
#define SUBTRACTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class SubtractNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Subtract" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Subtract" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SubtractNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SubtractNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

private:
	static QVariant subtractNumber( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
	static QVariant subtractVector3D( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
	static QVariant subtractPoint( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	QSharedPointer<fugio::PinInterface>			 mPinInput;
};

#endif // SUBTRACTNODE_H
