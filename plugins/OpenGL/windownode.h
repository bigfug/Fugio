#ifndef WINDOWNODE_H
#define WINDOWNODE_H

#include "opengl_includes.h"

#include <QWindow>
#include <QTimer>
#include <QAction>

#include <fugio/global_interface.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include "fugio/output_interface.h"
#include <fugio/core/variant_interface.h>
#include <fugio/image/image.h>
#include <fugio/gui/input_events_interface.h>

#include <fugio/opengl/uuid.h>

#include <fugio/nodecontrolbase.h>

#include "deviceopengloutput.h"

#include <fugio/render_interface.h>

#include <QUuid>

FUGIO_NAMESPACE_BEGIN
class OpenGLTextureInterface;
FUGIO_NAMESPACE_END

class WindowNode : public fugio::NodeControlBase, public fugio::OutputInterface, public fugio::RenderInterface, private QOpenGLFunctions
{
	Q_OBJECT
	Q_INTERFACES( fugio::OutputInterface fugio::RenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Opens an OpenGL window" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Window_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE WindowNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~WindowNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

//	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

//	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceOutput

	virtual void screengrab( const QString &pFileName = QString() ) Q_DECL_OVERRIDE;

	virtual void resize( int pWidth, int pHeight ) Q_DECL_OVERRIDE;

	virtual bool showCursor( void ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceOpenGLRenderer

	virtual void render( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private slots:
	void contextFrameInitialise( void );

	void contextFrameEnd( void );

	void outputResized(QSize pSize );

private:
	QSharedPointer<fugio::PinInterface>	 mPinInputTrigger;

	QSharedPointer<fugio::PinInterface>	 mPinState;

	QSharedPointer<fugio::PinInterface>	 mPinMouseShowCursor;

	QSharedPointer<fugio::PinInterface>	 mPinWindowSize;
	fugio::VariantInterface				*mValWindowSize;

	QSharedPointer<fugio::PinInterface>	 mPinOutputEvents;
	fugio::InputEventsInterface			*mValOutputEvents;

	QSharedPointer<fugio::PinInterface>	 mPinInputRender;

	QSharedPointer<DeviceOpenGLOutput>	 mOutput;

	QImage								 mBufferImage;

	qint64								 mTimeStamp;
	GLuint								 mGrabScreenId;

	QStringList							 mGrabFileNames;

	QPointF								 mMouseLastPosition;

	volatile bool						 mUpdate;
};

#endif // WINDOWNODE_H
