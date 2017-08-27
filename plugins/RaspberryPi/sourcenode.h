#ifndef SOURCENODE_H
#define SOURCENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/choice_interface.h>

#if defined( Q_OS_RASPBERRY_PI )
#include <IL/OMX_Component.h>
#endif

class SourceNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "OMX.broadcom.source" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Source_(OMX)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SourceNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SourceNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

#if defined( Q_OS_RASPBERRY_PI )
	static OMX_ERRORTYPE OMXEventHandler(
		 OMX_IN OMX_HANDLETYPE hComponent,
		 OMX_IN OMX_PTR pAppData,
		 OMX_IN OMX_EVENTTYPE eEvent,
		 OMX_IN OMX_U32 nData1,
		 OMX_IN OMX_U32 nData2,
		 OMX_IN OMX_PTR pEventData);

	static OMX_ERRORTYPE OMXEmptyBufferDone(
		OMX_IN OMX_HANDLETYPE hComponent,
		OMX_IN OMX_PTR pAppData,
		OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

	static OMX_ERRORTYPE OMXFillBufferDone(
		OMX_OUT OMX_HANDLETYPE hComponent,
		OMX_OUT OMX_PTR pAppData,
		OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);

#endif

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputFormat;

	fugio::ChoiceInterface						*mValInputFormat;

#if defined( Q_OS_RASPBERRY_PI )
	OMX_HANDLETYPE								 mSourceHandle;
	static OMX_CALLBACKTYPE						 mOMXCallbacks;
#endif
};

#endif // SOURCENODE_H
