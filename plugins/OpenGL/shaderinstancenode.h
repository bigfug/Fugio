#ifndef SHADERINSTANCENODE_H
#define SHADERINSTANCENODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/opengl/shader_interface.h>
#include <fugio/render_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/opengl/node_render_interface.h>

using namespace fugio;

class ShaderInstanceNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface
{
    Q_OBJECT
    Q_INTERFACES( fugio::NodeRenderInterface )

    Q_CLASSINFO( "Author", "Alex May" )
    Q_CLASSINFO( "Version", "1.0" )
    Q_CLASSINFO( "Description", "Uses a compiled OpenGL shader to draw geometry" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Shader_Instance_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ShaderInstanceNode( QSharedPointer<fugio::NodeInterface> pNode );

    virtual ~ShaderInstanceNode( void ) {}

    // NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QStringList availableInputPins() const Q_DECL_OVERRIDE;
	virtual QList<AvailablePinEntry> availableOutputPins() const Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

    // NodeControlInterface interface

	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

protected:
    void bindTexture( QList<fugio::ShaderBindData> &Bindings, QSharedPointer<PinControlInterface> PinControl, const fugio::ShaderUniformData &UniformData );
    int activeBufferCount(QList<QSharedPointer<fugio::PinInterface> > &OutPinLst) const;
    void bindOutputBuffers(QVector<GLenum> &Buffers, QList<QSharedPointer<fugio::PinInterface> > &OutPinLst, int &W, int &H, int &D);
	void releaseInputTextures(QList<ShaderBindData> &Bindings);

    void bindUniforms( QList<ShaderBindData> &Bindings );

protected:
    QSharedPointer<fugio::PinInterface>	 mPinState;
    QSharedPointer<fugio::PinInterface>	 mPinShader;
	QSharedPointer<fugio::PinInterface>	 mPinInputVAO;
	QSharedPointer<fugio::PinInterface>	 mPinInputPasses;
	QSharedPointer<fugio::PinInterface>	 mPinInputPassVar;

    QSharedPointer<fugio::PinInterface>	 mPinOutputGeometry;
	fugio::RenderInterface				*mOutputGeometry;

	GLuint								 mFrameBufferId;
};

#endif // SHADERINSTANCENODE_H
