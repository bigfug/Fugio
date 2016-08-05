#ifndef MULTIPLYNODE_H
#define MULTIPLYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class MultiplyNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Multiplies all the input numbers together." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Multiply_Number" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MultiplyNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MultiplyNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesInput() const;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const;

private:
	static QVariant multiplyNumber( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
	static QVariant multiplyVector3D( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
	static QVariant multiplyVector4D( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
	static QVariant multiplyPoint( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );
	static QVariant multiplyMatrix4x4( const QList< QSharedPointer<fugio::PinInterface> > pInputPins );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // MULTIPLYNODE_H
