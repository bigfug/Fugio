#ifndef BUFFERENTRYNODE_H
#define BUFFERENTRYNODE_H

#include "opengl_includes.h"

#include <QObject>
#include <QPair>
#include <QMap>

#include <fugio/nodecontrolbase.h>
#include <fugio/choice_interface.h>
#include <fugio/opengl/buffer_entry_interface.h>

#include "bufferentrypin.h"

class BufferEntryNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Type_Size" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BufferEntryNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BufferEntryNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputType;
	fugio::ChoiceInterface						*mValInputType;

	QSharedPointer<fugio::PinInterface>			 mPinInputCount;

	QSharedPointer<fugio::PinInterface>			 mPinOutputEntry;
	BufferEntryPin								*mValOutputEntry;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSize;
	fugio::VariantInterface						*mValOutputSize;

	typedef QPair<GLenum,GLsizei>				 TypeData;
	typedef QMap<QString,TypeData>				 TypeMap;

	static TypeMap								 mTypeMap;
};

#endif // BUFFERENTRYNODE_H
