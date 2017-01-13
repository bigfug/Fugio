#ifndef MATRIXTRANSLATENODE_H
#define MATRIXTRANSLATENODE_H

#include <QObject>
#include <QSharedPointer>
#include <QMatrix4x4>

#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class MatrixTranslateNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Create a translation Matrix4" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Translation_(Matrix4)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MatrixTranslateNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixTranslateNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinValue;
	fugio::VariantInterface						*mValue;

	QSharedPointer<fugio::PinInterface>			 mPinInputMatrix;
	QSharedPointer<fugio::PinInterface>			 mPinInputX;
	QSharedPointer<fugio::PinInterface>			 mPinInputY;
	QSharedPointer<fugio::PinInterface>			 mPinInputZ;
};

#endif // MATRIXTRANSLATENODE_H
