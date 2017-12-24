#ifndef TRANSFORMNODE_H
#define TRANSFORMNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class TransformNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates a transform" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Transform_(Math)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TransformNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TransformNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QStringList availableInputPins() const Q_DECL_OVERRIDE;
	virtual bool mustChooseNamedInputPin() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

private:
	void scale( QTransform &T, const QVariant &V );
	void translate( QTransform &T, const QVariant &V );
	void shear( QTransform &T, const QVariant &V );
	void rotate( QTransform &T, const QVariant &V );
	void transform( QTransform &T, const QVariant &V );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinOutputTransform;
	fugio::VariantInterface						*mValOutputTransform;
};

#endif // TRANSFORMNODE_H
