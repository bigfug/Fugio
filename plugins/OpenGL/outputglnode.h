#ifndef OUTPUT_H
#define OUTPUT_H

#include <QWindow>
#include <QTimer>
#include <QAction>

#include "fugio/interface_app.h"
#include "fugio/interface_node.h"
#include "fugio/interface_node_control.h"
#include "fugio/interface_pin.h"
#include "fugio/interface_pin_control.h"
#include "fugio/interface_output.h"
#include "fugio/interface_variant.h"
#include "fugio/interface_image.h"

#include "fugio/fugio-opengl.h"

#include <fugio/nodecontrolbase.h>

#include "deviceopengloutput.h"

#include <QUuid>

class InterfaceTexture;

class OutputGLNode : public NodeControlBase, public InterfaceOutput
{
	Q_OBJECT
	Q_INTERFACES( InterfaceOutput )

public:
	Q_INVOKABLE OutputGLNode( QSharedPointer<InterfaceNode> pNode );

	virtual ~OutputGLNode( void );

	//-------------------------------------------------------------------------
	// InterfaceNodeControl

	virtual QUuid uuid( void )
	{
		return( NID_OUTPUTGL );
	}

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual bool initialise( void );

	virtual bool deinitialise( void );

//	virtual void loadSettings( QSettings &pSettings );

//	virtual void saveSettings( QSettings &pSettings );

	//-------------------------------------------------------------------------
	// InterfaceOutput

	virtual QWindow *window( void )
	{
		return( mOutput );
	}

	virtual void screengrab( const QString &pFileName = QString() );

	virtual void resize( int pWidth, int pHeight );

	virtual void buttonLeftPressed( bool pPressed );

	virtual void cursorPosition( const QPointF &pCurPos );

	virtual bool showCursor( void ) const;

	//-------------------------------------------------------------------------

protected:
//	virtual void initialize( void );
//	virtual void resize( int pWidth, int pHeight );
//	virtual void renderNow( void );

//	void renderLater( void );

//	virtual void closeEvent( QCloseEvent *pEvent );

//	virtual void mouseDoubleClickEvent( QMouseEvent *pEvent );

//	virtual bool event( QEvent *event );

//	virtual void resizeEvent( QResizeEvent *pEvent );

//	void exposeEvent(QExposeEvent *event);

//	void mousePressEvent( QMouseEvent *pEvent );

//	void mouseReleaseEvent( QMouseEvent *pEvent );

//	void mouseMoveEvent( QMouseEvent *pEvent );

//	void keyReleaseEvent( QKeyEvent *pEvent );

	void drawTexture(InterfaceTexture *Texture);

signals:
	
public slots:
	void screengrabTimer( void );

private:
	QSharedPointer<InterfacePin>	 mPinState;

	QSharedPointer<InterfacePin>	 mTexturePin;
	QSharedPointer<InterfacePin>	 mPinOutputWidth;
	QSharedPointer<InterfacePin>	 mPinOutputHeight;

	QSharedPointer<InterfacePin>	 mPinMouseShowCursor;

	QSharedPointer<InterfacePin>	 mPinInputWindowWidth;
	QSharedPointer<InterfacePin>	 mPinInputWindowHeight;

	QSharedPointer<InterfacePin>	 mPinWidth;
	InterfaceVariant				*mPinWidthValue;

	QSharedPointer<InterfacePin>	 mPinHeight;
	InterfaceVariant				*mPinHeightValue;

	QSharedPointer<InterfacePin>	 mPinOutputImage;
	InterfaceImage					*mPinOutputImageValue;

	QSharedPointer<InterfacePin>	 mPinMouseButtonLeft;
	InterfaceVariant				*mMouseButtonLeft;

	QSharedPointer<InterfacePin>	 mPinMousePosition;
	InterfaceVariant				*mMousePosition;

	QSharedPointer<InterfacePin>	 mPinTexture;
	InterfaceTexture				*mTexture;

	DeviceOpenGLOutput				*mOutput;

	QOpenGLFramebufferObject		*mFBO;
	QOpenGLFramebufferObject		*mFBOAA;

	QImage							 mBufferImage;

	QTimer							 mTimer;
	qint64							 mTimeStamp;
	GLuint							 mGrabScreenId;
	QAction							*mActionSaveImage;

	QStringList						 mGrabFileNames;

	QPointF							 mMouseLastPosition;

	// InterfaceNodeControl interface
public:
	virtual QList<QUuid> pinAddTypesInput() const;
	virtual bool canAcceptPin(InterfacePin *pPin) const;
};

#endif // OUTPUT_H
