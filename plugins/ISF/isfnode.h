#ifndef ISFNODE_H
#define ISFNODE_H

#include "opengl_includes.h"

#include <QObject>
#include <QColor>
#include <QDir>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/opengl/node_render_interface.h>

#include <fugio/render_interface.h>

class ISFNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Interactive Shader Format" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "ISF" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ISFNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ISFNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// NodeRenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

protected:
	typedef enum ISFInputType
	{
		UNKNOWN = -1,
		EVENT,
		BOOL,
		LONG,
		FLOAT,
		POINT2D,
		IMAGE,
		COLOR,
		AUDIO,
		AUDIOFFT
	} ISFInputType;

	static ISFInputType isfType( QString Type );

	typedef struct ISFInput
	{
		ISFInput( void ) : mType( UNKNOWN ), mEventFlag( false ), mUniform( -1 ) {}

		ISFInput( ISFInputType pType ) : mType( pType ), mEventFlag( false ), mUniform( -1 ) {}

		ISFInputType		mType;
		bool				mEventFlag;
		GLint				mUniform;
	} ISFInput;

	QMap<QString,ISFInput> parseInputs( QJsonArray Inputs );

	typedef struct ISFImport
	{
		ISFImport( void ) : mTextureId( 0 ), mUniform( -1 ) {}

		QString				mPath;
		GLuint				mTextureId;
		GLint				mUniform;
	} ISFImport;

	QMap<QString,ISFImport> parseImports( const QDir &pDir, const QJsonObject Imports ) const;

	void parseISF( const QDir &pDir, QByteArray Source );

private:
	bool loadShaders( const QString &pShaderSource );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputSource;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRender;
	fugio::RenderInterface						*mValOutputRender;

	QMap<QString,ISFInput>						 mISFInputs;
	QMap<QString,ISFImport>						 mISFImports;

	GLuint										 mVAO;
	GLuint										 mBuffer;
	GLuint										 mProgram;
	GLuint										 mFrameCounter;
	int											 mPasses;

	GLint										 mUniformTime;
	GLint										 mUniformRenderSize;
	GLint										 mUniformDate;
	GLint										 mUniformPassIndex;
	GLint										 mUniformTimeDelta;
	GLint										 mUniformFrameIndex;

	qint64										 mStartTime;
	qint64										 mLastRenderTime;
};

#endif // ISFNODE_H
