#include "sourcenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include <IL/OMX_Broadcom.h>
#include <IL/OMX_Component.h>

OMX_CALLBACKTYPE	SourceNode::mOMXCallbacks;

SourceNode::SourceNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FORMAT,	"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	//FUGID( PIN_OUTPUT_TRIGGER,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mValInputFormat = pinInput<fugio::ChoiceInterface *>( "Format", mPinInputFormat, PID_CHOICE, PIN_INPUT_FORMAT );

	mValInputFormat->setChoices( QStringList() << "RGB565" << "RGB888" << "ABGR8888" << "YUV420" << "YUV422" );

#if defined( Q_OS_RASPBERRY_PI )
	mSourceHandle = Q_NULLPTR;

	mOMXCallbacks.EventHandler    = &SourceNode::OMXEventHandler;
	mOMXCallbacks.EmptyBufferDone = &SourceNode::OMXEmptyBufferDone;
	mOMXCallbacks.FillBufferDone  = &SourceNode::OMXFillBufferDone;
#endif

	//OMX_PARAM_PORTDEFINITIONTYPE
}

bool SourceNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

#if defined( Q_OS_RASPBERRY_PI )
	OMX_ERRORTYPE	OMXErr;

	if( ( OMXErr = OMX_GetHandle( &mSourceHandle, "OMX.broadcom.source", this, &mOMXCallbacks ) ) != OMX_ErrorNone )
	{
		qDebug() << "OMX_GetHandle" << QString::number( OMXErr, 16 );

		return( false );
	}

	OMX_PARAM_PORTDEFINITIONTYPE	ParamPortDef;

	memset( &ParamPortDef, 0, sizeof( ParamPortDef ) );

	ParamPortDef.nSize = sizeof( ParamPortDef );
	ParamPortDef.nVersion.nVersion = OMX_VERSION;

	ParamPortDef.nPortIndex = 20;

	if( ( OMXErr = OMX_GetParameter( mSourceHandle, OMX_IndexParamPortDefinition, &ParamPortDef ) ) != OMX_ErrorNone )
	{
		qDebug() << "OMX_IndexParamPortDefinition" << QString::number( OMXErr, 16 );
	}

//	ParamPortDef.format.video.eColorFormat = OMX_COLOR_Format24bitRGB888;
//	ParamPortDef.format.video.nFrameWidth  = 640;
//	ParamPortDef.format.video.nFrameHeight = 480;

//	if( ( OMXErr = OMX_SetParameter( mSourceHandle, OMX_IndexParamPortDefinition, &ParamPortDef ) ) != OMX_ErrorNone )
//	{
//		qDebug() << "OMX_IndexParamPortDefinition" << QString::number( OMXErr, 16 );
//	}

	OMX_PARAM_SOURCETYPE	SourceType;

	memset( &SourceType, 0, sizeof( SourceType ) );

	SourceType.nSize = sizeof( SourceType );
	SourceType.nVersion.nVersion = OMX_VERSION;

	SourceType.eType       = OMX_SOURCE_DIAGONAL;
	SourceType.nPortIndex  = 20;

	if( ( OMXErr = OMX_SetParameter( mSourceHandle, OMX_IndexParamSource, &SourceType ) ) != OMX_ErrorNone )
	{
		qDebug() << "OMX_IndexParamSource" << QString::number( OMXErr, 16 );
	}

	OMX_IMAGE_PARAM_PORTFORMATTYPE	PortFormatType;

	memset( &PortFormatType, 0, sizeof( PortFormatType ) );

	PortFormatType.nSize             = sizeof( PortFormatType );
	PortFormatType.nVersion.nVersion = OMX_VERSION;
	PortFormatType.nPortIndex        = 20;

	if( ( OMXErr = OMX_GetParameter( mSourceHandle, OMX_IndexParamImagePortFormat, &PortFormatType ) ) != OMX_ErrorNone )
	{
		qDebug() << "OMX_IndexParamImagePortFormat" << QString::number( OMXErr, 16 );
	}

	if( ( OMXErr = OMX_SendCommand( mSourceHandle, OMX_CommandStateSet, OMX_StateIdle, NULL ) ) != OMX_ErrorNone )
	{
		qDebug() << "OMX_CommandStateSet" << QString::number( OMXErr, 16 );
	}

	OMX_CONFIG_BRCMPORTSTATSTYPE	PortStats;

	memset( &PortStats, 0, sizeof( PortStats ) );

	PortStats.nSize             = sizeof( PortStats );
	PortStats.nVersion.nVersion = OMX_VERSION;
	PortStats.nPortIndex        = 20;

	if( ( OMXErr = OMX_GetParameter( mSourceHandle, OMX_IndexConfigBrcmPortStats, &PortStats ) ) != OMX_ErrorNone )
	{
		qDebug() << "OMX_IndexConfigBrcmPortStats" << QString::number( OMXErr, 16 );
	}

	if( ( OMXErr = OMX_SendCommand( mSourceHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL ) ) != OMX_ErrorNone )
	{
		qDebug() << "OMX_CommandStateSet" << QString::number( OMXErr, 16 );
	}

#endif

	return( true );
}

bool SourceNode::deinitialise()
{
#if defined( Q_OS_RASPBERRY_PI )
	if( mSourceHandle )
	{
		OMX_FreeHandle( mSourceHandle );

		mSourceHandle = Q_NULLPTR;
	}
#endif

	return( NodeControlBase::deinitialise() );
}

OMX_ERRORTYPE SourceNode::OMXEventHandler( OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	qDebug() << "OMXEventHandler" << hComponent << pAppData << eEvent << nData1 << nData2 << pEventData;

	return( OMX_ErrorNone );
}

OMX_ERRORTYPE SourceNode::OMXEmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE *pBuffer)
{
	qDebug() << "OMXEmptyBufferDone" << hComponent << pAppData << pBuffer;

	return( OMX_ErrorNone );
}

OMX_ERRORTYPE SourceNode::OMXFillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE *pBuffer)
{
	qDebug() << "OMXFillBufferDone" << hComponent << pAppData << pBuffer;

	return( OMX_ErrorNone );
}

void SourceNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

//	pinUpdated( mPinOutput );
}

