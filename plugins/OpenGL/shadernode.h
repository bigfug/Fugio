#ifndef SHADERNODE_H
#define SHADERNODE_H

#include <QObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_3_3_Compatibility>

#include "fugio/interface_node.h"
#include "fugio/interface_node_control.h"
#include "fugio/interface_pin.h"
#include "fugio/interface_pin_control.h"

#include "fugio/interface_texture.h"

#include "fugio/fugio-opengl.h"

#include <fugio/interface_geometry.h>

#include <fugio/interface_syntax_highlighter.h>

#include <fugio/nodecontrolbase.h>

class ShaderNode : public NodeControlBase, public InterfaceSyntaxHighlighter, public InterfaceGeometry, protected QOpenGLFunctions_3_3_Compatibility
{
	Q_OBJECT
	Q_INTERFACES( InterfaceSyntaxHighlighter InterfaceGeometry )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Old Shader" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Shader_OpenGL" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ShaderNode( QSharedPointer<InterfaceNode> pNode );

	virtual ~ShaderNode( void );

	//-------------------------------------------------------------------------
	// InterfaceNodeControl

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual bool initialise( void );

	virtual bool deinitialise( void );

	virtual void loadSettings( QSettings &pSettings );

	virtual void saveSettings( QSettings &pSettings );

	virtual QStringList availableInputPins() const;
	virtual QList<QUuid> pinAddTypesInput() const;
	virtual bool canAcceptPin(InterfacePin *pPin) const;

	//-------------------------------------------------------------------------
	// InterfaceGeometry

	virtual void drawGeometry( void );

	//-------------------------------------------------------------------------
	// InterfaceSyntaxHighlighter

	virtual QSyntaxHighlighter *highlighter( QTextDocument *pDocument );

	virtual QStringList errorList( int ) const
	{
		return( QStringList() );
	}

signals:
	void updateUniformList( QStringList pUniforms );

protected slots:
	void uniformAdd( QString pUniform );

private:
	typedef struct ShaderUniformData
	{
		GLenum							 mType;
		GLint							 mSize;
		GLint							 mTextureBinding;
		GLint							 mLocation;
		GLboolean						 mSampler;
	} ShaderUniformData;

	typedef struct ShaderBindData
	{
		GLenum							  mUnit;
		InterfaceTexture				 *mTexture;
	} ShaderBindData;

	typedef QMap<QString, ShaderUniformData>	 ShaderUniformMap;

	void loadShader( void );

	GLint loadShader( QSharedPointer<InterfacePin> pPin, GLuint &pShaderId, GLenum pShaderType );

	void clearShader( void );

	void processShader( GLuint pProgramId );

	void bindOutputBuffers( QVector<GLenum> &Buffers, QList< QSharedPointer<InterfacePin> > &OutPinLst, int &W, int &H, int &D );

	void updateUniforms( QList<ShaderBindData> &Bindings, qint64 pTimeStamp);

	void bindAttributes( void );

	void releaseAttributes( void );

	void bindInputTextures( QList<ShaderBindData> &Bindings );

	void bindTexture( QList<ShaderBindData> &Bindings, QSharedPointer<InterfacePinControl> PinControl, ShaderUniformData &UniformData );

	void releaseInputTextures( QList<ShaderBindData> &Bindings );

	int activeBufferCount( QList< QSharedPointer<InterfacePin> > &OutPinLst ) const;

private:
	QSharedPointer<InterfacePin>		 mPinShaderVertex;
	QSharedPointer<InterfacePin>		 mPinShaderGeometry;
	QSharedPointer<InterfacePin>		 mPinShaderTessCtrl;
	QSharedPointer<InterfacePin>		 mPinShaderTessEval;
	QSharedPointer<InterfacePin>		 mPinShaderFragment;
	QSharedPointer<InterfacePin>		 mPinGeometry;
	QSharedPointer<InterfacePin>		 mPinState;

	QSharedPointer<InterfacePin>		 mPinOutputGeometry;
	InterfaceGeometry					*mOutputGeometry;

	QSharedPointer<InterfacePin>		 mPinOutputTexture0;
	InterfaceTexture					*mOutputTexture0;

	ShaderUniformMap					 mShaderUniformTypes;
	ShaderUniformMap					 mShaderAttributeTypes;
	GLuint								 mProgramId;
	GLboolean							 mProgramLinked;
	QStringList							 mUniformNames;
	QStringList							 mAttributeNames;
	GLuint								 mFrameBufferId;
	qint64								 mLastShaderLoad;
	GLuint								 mShaderVertId;
	GLuint								 mShaderGeomId;
	GLuint								 mShaderTessCtrlId;
	GLuint								 mShaderTessEvalId;
	GLuint								 mShaderFragId;

	bool								 mInitialised;
};

#endif // SHADERNODE_H
