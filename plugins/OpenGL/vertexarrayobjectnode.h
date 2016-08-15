#ifndef VERTEXARRAYOBJECTNODE_H
#define VERTEXARRAYOBJECTNODE_H

#include "opengl_includes.h"

#include <fugio/nodecontrolbase.h>

#include "vertexarrayobjectpin.h"

class VertexArrayObjectNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Vertex Array Object" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Vertex_Array_Object_OpenGL" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

	void freeVAO();

public:
	Q_INVOKABLE VertexArrayObjectNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~VertexArrayObjectNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual QStringList availableInputPins() const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputShader;

	QSharedPointer<fugio::PinInterface>			 mPinOutputVAO;
	VertexArrayObjectPin						*mValOutputVAO;

	GLuint										 mVAO;

	typedef struct BindInfo
	{
		GLint			 mLocation;
		QMetaType::Type	 mMetaType;
	} BindInfo;

	typedef QMap<QString,BindInfo> BindInfoMap;

	BindInfoMap									 mBindInfo;
};

#endif // VERTEXARRAYOBJECTNODE_H
