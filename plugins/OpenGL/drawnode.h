#ifndef DRAWNODE_H
#define DRAWNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/opengl/node_render_interface.h>

class DrawNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface, protected QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/OpenGL_Draw" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE DrawNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DrawNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

	// fugio::NodeRenderInterface interface

	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId );

	static bool bufferTypeSize( QMetaType::Type pMetaType, GLint &pSize, GLenum &pType );

protected:
	bool processBuffer( int pIndex, QSharedPointer<fugio::PinInterface> &pPin, int &pCount );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinCount;
	QSharedPointer<fugio::PinInterface>			 mPinInstances;
	QSharedPointer<fugio::PinInterface>			 mPinInputIndex;

	QSharedPointer<fugio::PinInterface>			 mPinMode;
	fugio::ChoiceInterface						*mValMode;

	static QMap<QString,GLenum>					 mModeMap;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRender;
	fugio::RenderInterface						*mValOutputRender;
};

#endif // DRAWNODE_H
