#ifndef BUFFERTOARRAYNODE_H
#define BUFFERTOARRAYNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/array_interface.h>

class BufferToArrayNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Copies an OpenGL buffer to an array" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Buffer_To_Array" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BufferToArrayNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BufferToArrayNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise();

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinBuffer;

	QSharedPointer<fugio::PinInterface>			 mPinArray;
	fugio::ArrayInterface						*mValArray;
};


#endif // BUFFERTOARRAYNODE_H
