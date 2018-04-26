#ifndef ISFNODE_H
#define ISFNODE_H

#if 1// !defined( Q_OS_RASPBERRY_PI )
#include "../libs/exprtk/exprtk.hpp"
#endif

#include "opengl_includes.h"

#include <QObject>
#include <QColor>
#include <QDir>
#include <QSize>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/opengl/node_render_interface.h>

#include <fugio/render_interface.h>

#include <fugio/audio/audio_instance_base.h>

#include <fugio/file/filename_interface.h>

class ISFNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface, private QOpenGLFunctions
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

	virtual bool initialise() Q_DECL_OVERRIDE;

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
		FFT
	} ISFInputType;

	static ISFInputType isfType( QString Type );

	typedef struct ISFInput
	{
		ISFInput( void ) : mType( UNKNOWN ), mEventFlag( false ), mUniform( -1 ), mTextureIndex( -1 ), mTextureId( 0 ), mAudioInstance( nullptr ), mSamplePosition( 0 ) {}

		ISFInput( ISFInputType pType ) : mType( pType ), mEventFlag( false ), mUniform( -1 ), mTextureIndex( -1 ), mTextureId( 0 ), mAudioInstance( nullptr ), mSamplePosition( 0 ) {}

		ISFInputType				 mType;
		bool						 mEventFlag;
		GLint						 mUniform;
		GLint						 mTextureIndex;
		GLuint						 mTextureId;
		int							 mAudioMax;

		fugio::AudioInstanceBase	*mAudioInstance;
		qint64						 mSamplePosition;
		qint64						 mAudioLatency;

		QVector<float>				 mAudioData;

	} ISFInput;

	QMap<QString,ISFInput> parseInputs( QJsonArray Inputs );

	typedef struct ISFImport
	{
		ISFImport( void ) : mTextureId( 0 ), mUniform( -1 ), mTextureIndex( -1 ) {}

		QString				mPath;
		GLuint				mTextureId;
		GLint				mUniform;
		GLint				mTextureIndex;
	} ISFImport;

	QMap<QString,ISFImport> parseImports( const QDir &pDir, const QJsonObject Imports ) const;

	void parseISF( const QDir &pDir, QByteArray Source );

	typedef struct ISFPass
	{
		ISFPass( void ) : mPersistent( false ), mFloat( false ), mFBO( 0 ), mTextureId( 0 ), mUniform( -1 ), mTextureIndex( -1 ) {}

		QString				mTarget;
		bool				mPersistent;
		bool				mFloat;
		QString				mWidth;
		QString				mHeight;

		GLuint				mFBO;
		GLuint				mTextureId;
		QSize				mSize;
		GLint				mUniform;
		GLint				mTextureIndex;
	} ISFPass;

	QList<ISFPass> parsePasses( const QJsonArray pPasses ) const;

	void renderInputs();

	void renderImports();

	void renderPasses( GLint Viewport[ 4 ] );

	void renderUniforms( qint64 pTimeStamp, GLint Viewport[ 4 ] );

	int calculateValue( int pValue, const QSize &pSize, QString pExpression ) const;

private:
	bool loadShaders( const QString &pShaderSource );

	static float getBandWidth( float timeSize, float sampleRate );

	static int freqToIndex( int timeSize, int sampleRate, int freq);

private slots:
	void contextProcess( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputFilename;
	fugio::FilenameInterface					*mValInputFilename;

	QSharedPointer<fugio::PinInterface>			 mPinInputSource;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRender;
	fugio::RenderInterface						*mValOutputRender;

	QMap<QString,ISFInput>						 mISFInputs;
	QMap<QString,ISFImport>						 mISFImports;
	QList<ISFPass>								 mISFPasses;

	QOpenGLVertexArrayObject					 mVAO;
	QOpenGLBuffer								 mBuffer;
	GLuint										 mProgram;
	GLuint										 mFrameCounter;

	GLint										 mUniformTime;
	GLint										 mUniformRenderSize;
	GLint										 mUniformDate;
	GLint										 mUniformPassIndex;
	GLint										 mUniformTimeDelta;
	GLint										 mUniformFrameIndex;

	GLuint										 mTextureIndexCount;

	qint64										 mStartTime;
	qint64										 mLastRenderTime;
};

#endif // ISFNODE_H
