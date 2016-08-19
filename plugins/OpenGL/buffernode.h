#ifndef BUFFERNODE_H
#define BUFFERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/opengl/buffer_entry_interface.h>

class BufferEntryProxyPin;

class BufferNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Buffer_OpenGL" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BufferNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BufferNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;
	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid(QSharedPointer<fugio::PinInterface> pPin) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputBuffer;
	QSharedPointer<fugio::PinInterface>			 mPinInputEntry;

	QSharedPointer<fugio::PinInterface>			 mPinOutputEntry;
	BufferEntryProxyPin							*mValOutputEntry;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSize;
	fugio::VariantInterface						*mValOutputSize;

	GLuint										 mEntrySize;
};

#endif // BUFFERNODE_H
