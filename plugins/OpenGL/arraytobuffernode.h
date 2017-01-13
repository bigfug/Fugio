#ifndef ARRAYTOBUFFERNODE_H
#define ARRAYTOBUFFERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/paired_pins_helper_interface.h>

#include <fugio/opengl/buffer_interface.h>

FUGIO_NAMESPACE_BEGIN
class VariantInterface;
FUGIO_NAMESPACE_END

class ArrayToBufferNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Copies an array of data to the graphics card for processing with OpenGL shaders" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Array_To_Buffer_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ArrayToBufferNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ArrayToBufferNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputArray;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBuffer;
	fugio::OpenGLBufferInterface				*mValOutputBuffer;
};

#endif // ARRAYTOBUFFERNODE_H
