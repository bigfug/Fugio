#ifndef ISFNODE_H
#define ISFNODE_H

#include "opengl_includes.h"

#include <QObject>
#include <QColor>

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
		ISFInput( void ) : mType( UNKNOWN ), mEventFlag( false ) {}

		ISFInput( ISFInputType pType ) : mType( pType ), mEventFlag( false ) {}

		ISFInputType	mType;
		bool			mEventFlag;
	} ISFInput;

	QMap<QString,ISFInput> parseInputs( QJsonArray Inputs );

private:
	void loadShaders( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputSource;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRender;
	fugio::RenderInterface						*mValOutputRender;

	QString										 mShaderSource;

	QMap<QString,ISFInput>						 mISFInputs;

	GLuint										 mVAO;
	GLuint										 mBuffer;
	GLuint										 mProgram;

	qint64										 mLastRenderTime;
};

#endif // ISFNODE_H
