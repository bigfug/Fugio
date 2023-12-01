#ifndef VERTEXARRAYOBJECTNODE_H
#define VERTEXARRAYOBJECTNODE_H

#include "opengl_includes.h"

#include <fugio/nodecontrolbase.h>

#include "vertexarrayobjectpin.h"

#include <fugio/opengl/context_container_object.h>

FUGIO_NAMESPACE_BEGIN
class OpenGLBufferInterface;
struct ShaderUniformData;
FUGIO_NAMESPACE_END

class VertexArrayObjectNode : public fugio::NodeControlBase, public fugio::VertexArrayObjectInterface, protected QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::VertexArrayObjectInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Vertex Array Object" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Vertex_Array_Object_(OpenGL)" ) )
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

	// VertexArrayObjectInterface interface
public:
	virtual void vaoBind() Q_DECL_OVERRIDE;
	virtual void vaoRelease() Q_DECL_OVERRIDE;
	virtual GLuint vaoId() const Q_DECL_OVERRIDE;

private:
	void bindPin( fugio::PinInterface *P, fugio::PinControlInterface *PinControl, fugio::OpenGLBufferInterface *Buffer, const fugio::ShaderUniformData &UniformData );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputShader;

	QSharedPointer<fugio::PinInterface>			 mPinOutputVAO;
	VertexArrayObjectPin						*mValOutputVAO;

	typedef struct BindInfo
	{
		GLint			 mLocation;
		QMetaType::Type	 mMetaType;
	} BindInfo;

	typedef QMap<QString,BindInfo>					BindInfoMap;
	typedef QMap<QOpenGLContext *,BindInfoMap>		ContextBindInfoMap;

	fugio::ContextVertexArrayObject				 mVAO;
	ContextBindInfoMap							 mBindInfo;
};

#endif // VERTEXARRAYOBJECTNODE_H
