#ifndef EASYSHADER2DNODE_H
#define EASYSHADER2DNODE_H

#include <QObject>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/text/syntax_error_interface.h>

#include "shadercompilerdata.h"

FUGIO_NAMESPACE_BEGIN
class VariantInterface;
FUGIO_NAMESPACE_END

class EasyShader2DNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface, private QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Easy OpenGL 2D Shader" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Easy_Shader_2D_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE EasyShader2DNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~EasyShader2DNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE
	{
		return( QList<QUuid>() );
	}

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE
	{
		return( pPin->direction() == PIN_OUTPUT );
	}

	// RenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

private:
	void renderToTexture( qint64 pTimeStamp );

	void compileShader( void );

	void createInputPins( void );

	void createOutputPins( void );

	void updateInputPins( void );

	void updateOutputPins( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;

	QSharedPointer<fugio::PinInterface>			 mPinInputShaderVertex;
	fugio::SyntaxErrorInterface					*mValInputShaderVertex;

	QSharedPointer<fugio::PinInterface>			 mPinInputShaderFragment;
	fugio::SyntaxErrorInterface					*mValInputShaderFragment;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRender;
	fugio::RenderInterface						*mValOutputRender;

	QOpenGLVertexArrayObject					 mVAO;

	ShaderCompilerData							 mShaderCompilerData;

	QOpenGLBuffer								 mQuadGeometry;

	GLuint										 mFramebufferObject;
	QSize										 mFramebufferSize;
};

#endif // EASYSHADER2DNODE_H
