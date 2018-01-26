#include "vst3node.h"

#include <QMainWindow>

#include <fugio/context_interface.h>
#include <fugio/global_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/audio/uuid.h>
#include <fugio/performance.h>
#include <fugio/portmidi/uuid.h>
#include <fugio/midi/midi_interface.h>
#include <fugio/midi/uuid.h>
#include <fugio/pin_signals.h>
#include <fugio/editor_interface.h>

#include "vstplugin.h"
#include "vstview.h"

#include "dirlocker.h"

#if defined( VST_SUPPORTED )

#if defined( Q_OS_WIN )
#include <windows.h>
#include <conio.h>
extern "C"
{
	typedef bool (PLUGIN_API *InitModuleProc) ();
	typedef bool (PLUGIN_API *ExitModuleProc) ();
}
static const Steinberg::FIDString kInitModuleProcName = "InitDll";
static const Steinberg::FIDString kExitModuleProcName = "ExitDll";

#elif defined( Q_OS_MAC )
//#include <CoreFoundation/CoreFoundation.h>
#include <curses.h>
typedef bool (*bundleEntryPtr)(CFBundleRef);
typedef bool (*bundleExitPtr)(void);

#endif

//#include "pluginterfaces/test/itest.h"
#include "pluginterfaces/base/ipluginbase.h"

#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstcontextmenu.h"
#include "pluginterfaces/vst/ivstunits.h"

#include "base/source/fstring.h"
//#include "base/source/fcontainer.h"
//#include "base/source/fmemory.h"
#include "base/source/fstreamer.h"
//#include "base/source/tarray.h"
//#include "base/source/tdictionary.h"
#include "base/source/fcommandline.h"
#include "pluginterfaces/base/ibstream.h"

#include "public.sdk/source/common/memorystream.h"

#include <pluginterfaces/gui/iplugview.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

#endif // defined( VST_SUPPORTED )

#if defined( VST_SUPPORTED )

namespace Steinberg {
namespace Vst {

//--------------------------------------------------------------------------------------------------------------

IMPLEMENT_FUNKNOWN_METHODS (ParameterValueQueue, IParamValueQueue, IParamValueQueue::iid)

const int32 kQueueReservedPoints = 5;

//--------------------------------------------------------------------------------------------------------------
ParameterValueQueue::ParameterValueQueue (ParamID paramID)
: paramID (paramID)
{
	values.reserve (kQueueReservedPoints);
	FUNKNOWN_CTOR
}

//--------------------------------------------------------------------------------------------------------------
ParameterValueQueue::~ParameterValueQueue ()
{
	FUNKNOWN_DTOR
}

//--------------------------------------------------------------------------------------------------------------
void ParameterValueQueue::clear ()
{
	values.clear ();
}

//--------------------------------------------------------------------------------------------------------------
int32 PLUGIN_API ParameterValueQueue::getPointCount ()
{
	//return (int32)values.size ();

	return( 1 );
}

//--------------------------------------------------------------------------------------------------------------
tresult PLUGIN_API ParameterValueQueue::getPoint (int32 index, int32& sampleOffset, ParamValue& value)
{
//	if (index < (int32)values.size ())
//	{
//		const ParameterQueueValue& queueValue = values[index];
//		sampleOffset = queueValue.sampleOffset;
//		value = queueValue.value;
//		return kResultTrue;
//	}
//	return kResultFalse;

	sampleOffset = 0;
	value = 0;

	if( mPin->isConnected() )
	{
		fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( mPin->connectedPin()->control()->qobject() );

		if( V )
		{
			value = V->variant().toDouble();
		}
	}
	else
	{
		value = mPin->value().toDouble();
	}

	return kResultTrue;
}

//--------------------------------------------------------------------------------------------------------------
tresult PLUGIN_API ParameterValueQueue::addPoint (int32 sampleOffset, ParamValue value, int32& index)
{
//	int32 destIndex = (int32)values.size ();
//	for (uint32 i = 0; i < values.size (); i++)
//	{
//		if (values[i].sampleOffset == sampleOffset)
//		{
//			values[i].value = value;
//			index = i;
//			return kResultTrue;
//		}
//		else if (values[i].sampleOffset > sampleOffset)
//		{
//			destIndex = i;
//			break;
//		}
//	}

//	// need new point
//	ParameterQueueValue queueValue (value, sampleOffset);
//	if (destIndex == (int32)values.size ())
//		values.push_back (queueValue);
//	else
//		values.insert (values.begin () + destIndex, queueValue);

//	index = destIndex;

	return kResultTrue;
}

}}

#endif

VST3Node::VST3Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
#if defined( VST_SUPPORTED )
	, mPluginFactory( nullptr ), mPluginComponent( nullptr ), mPluginController( nullptr ), mPluginView( nullptr )
	, mDockWidget( nullptr ), mDockArea( Qt::BottomDockWidgetArea )
#endif
{
}

VST3Node::~VST3Node( void )
{

}

#if defined( VST_SUPPORTED )
bool VST3Node::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	QString			LibNam;
	int				ClsIdx;

	if( !VSTPlugin::findPluginInfo( mNode->controlUuid(), LibNam, ClsIdx ) )
	{
		return( false );
	}

#if defined( Q_OS_MACX )
	QDir			DirTmp = QDir( LibNam );

	DirTmp.cdUp();
	DirTmp.cdUp();
	DirTmp.cd( "Resources" );

	DirLocker		DirLck( DirTmp.absolutePath() );
#else
	DirLocker		DirLck( QDir( LibNam ).absolutePath() );
#endif

#if defined( VST_SUPPORTED )

	mPluginLibrary.setFileName( LibNam );

	if( !mPluginLibrary.load() )
	{
		return( false );
	}

#if defined( Q_OS_WIN )
	InitModuleProc InitModuleProcFunc = (InitModuleProc)mPluginLibrary.resolve( kInitModuleProcName );

	if( InitModuleProcFunc && !InitModuleProcFunc() )
	{
		return( false );
	}
#endif

	GetFactoryProc GetFactoryProcFunc = (GetFactoryProc)mPluginLibrary.resolve( "GetPluginFactory" );

	if( !GetFactoryProcFunc )
	{
		return( false );
	}

	if( !( mPluginFactory = GetFactoryProcFunc() ) )
	{
		return( false );
	}

	Steinberg::PFactoryInfo					 mPluginFactoryInfo;

	mPluginFactory->getFactoryInfo( &mPluginFactoryInfo );

	Steinberg::PClassInfo2 classInfo;
	Steinberg::IPluginFactory2* factory2;

	if (mPluginFactory->queryInterface (Steinberg::IPluginFactory2::iid, (void**)&factory2) == Steinberg::kResultTrue)
	{
		factory2->getClassInfo2( ClsIdx, &classInfo);
		factory2->release ();
	}

	Steinberg::IPluginFactory3* factory3;

	if (mPluginFactory->queryInterface (Steinberg::IPluginFactory3::iid, (void**)&factory3) == Steinberg::kResultTrue)
	{
		factory3->setHostContext( getFUnknown() );

		factory3->release ();
	}

	bool res = false;
	tresult result;

	try
	{
		qDebug() << QDir::currentPath();

		result = mPluginFactory->createInstance( classInfo.cid, Vst::IComponent::iid, (void**)&mPluginComponent );

		if( !mPluginComponent || ( result != kResultOk ) )
		{
			return( false );
		}
	}
	catch( ... )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		qWarning() << LibNam << "- createInstance failed";

		return( false );
	}

	// initialize the component with our context
	res = ( mPluginComponent->initialize( getFUnknown() ) == kResultOk );

	// try to create the controller part from the component
	// (for Plug-ins which did not succeed to separate component from controller)
	if( mPluginComponent->queryInterface( Vst::IEditController::iid, (void**)&mPluginController) != kResultTrue )
	{
		TUID controllerCID;

		// ask for the associated controller class ID
		if (mPluginComponent->getControllerClassId (controllerCID) == kResultTrue )
		{
			// create its controller part created from the factory
			result = mPluginFactory->createInstance (controllerCID, Vst::IEditController::iid, (void**)&mPluginController);

			if (mPluginController && (result == kResultOk))
			{
				// initialize the component with our context
				res = (mPluginController->initialize ( getFUnknown() ) == kResultOk);

				if( res )
				{
					mPluginController->setComponentHandler( this );
				}
			}
		}
	}

	if( mPluginController )
	{
		fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

		if( EI )
		{
			Steinberg::IPlugView					*PluginView;

			PluginView = mPluginController->createView( Steinberg::Vst::ViewType::kEditor );

			if( PluginView )
			{
				if( PluginView->isPlatformTypeSupported( Steinberg::kPlatformTypeHWND ) == kResultTrue )
				{
					if( ( mDockWidget = new QDockWidget( QString( "VST: %1" ).arg( mNode->name() ), EI->mainWindow() ) ) )
					{
						mDockWidget->setWidget( new VSTView( PluginView ) );

						mDockWidget->setObjectName( mNode->name() );

						EI->mainWindow()->addDockWidget( mDockArea, mDockWidget );

						mDockWidget->show();
					}
				}
			}
		}
	}

	if( res )
	{
		for( int i = 0 ; i < mPluginComponent->getBusCount( Vst::kEvent, Vst::kInput ) ; i++ )
		{
			Vst::BusInfo		BI;

			if( mPluginComponent->getBusInfo( Vst::kEvent, Vst::kInput, i, BI ) == kResultTrue )
			{
				QString		PinNam = QString( (const QChar *)&BI.name );

				QSharedPointer<fugio::PinInterface>	 IP = mNode->findInputPinByName( PinNam );

				if( !IP )
				{
					IP = pinInput( PinNam, QUuid::createUuid() );

					IP->registerPinInputType( PID_MIDI_OUTPUT );
				}

				mEventInputs.insert( i, IP );
			}
		}

		for( int i = 0 ; i < mPluginComponent->getBusCount( Vst::kAudio, Vst::kInput ) ; i++ )
		{
			Vst::BusInfo		BI;

			if( mPluginComponent->getBusInfo( Vst::kAudio, Vst::kInput, i, BI ) == kResultTrue )
			{
				QString		PinNam = QString( (const QChar *)&BI.name );

				QSharedPointer<fugio::PinInterface>	 IP = mNode->findInputPinByName( PinNam );

				if( !IP )
				{
					IP = pinInput( PinNam, QUuid::createUuid() );

					IP->registerPinInputType( PID_AUDIO );
				}

				if( IP )
				{
					connect( IP->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(audioLinked(QSharedPointer<fugio::PinInterface>)) );
					connect( IP->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(audioUnlinked(QSharedPointer<fugio::PinInterface>)) );
				}

				mAudioInputs.insert( i, IP );
			}
		}

		for( int i = 0 ; i < mPluginComponent->getBusCount( Vst::kEvent, Vst::kOutput ) ; i++ )
		{
			Vst::BusInfo		BI;

			if( mPluginComponent->getBusInfo( Vst::kEvent, Vst::kOutput, i, BI ) == kResultTrue )
			{
				QString		PinNam = QString( (const QChar *)&BI.name );

				QSharedPointer<fugio::PinInterface>	 IP = mNode->findOutputPinByName( PinNam );
				fugio::MidiInterface						*IM;

				if( !IP )
				{
					IM = pinOutput<fugio::MidiInterface *>( QString( (const QChar *)&BI.name ), IP, PID_MIDI_OUTPUT, QUuid::createUuid() );
				}
				else
				{
					IM = qobject_cast<fugio::MidiInterface *>( IP->control()->qobject() );
				}

				//mEventOutputs.insert( i, AudioPinPair( IP, IA ) );
			}
		}

		for( int i = 0 ; i < mPluginComponent->getBusCount( Vst::kAudio, Vst::kOutput ) ; i++ )
		{
			Vst::BusInfo		BI;

			if( mPluginComponent->getBusInfo( Vst::kAudio, Vst::kOutput, i, BI ) == kResultTrue )
			{
				QString		PinNam = QString( (const QChar *)&BI.name );

				QSharedPointer<fugio::PinInterface>	 IP = mNode->findOutputPinByName( PinNam );
				fugio::AudioProducerInterface		*IA;

				if( !IP )
				{
					IA = pinOutput<fugio::AudioProducerInterface *>( QString( (const QChar *)&BI.name ), IP, PID_AUDIO, QUuid::createUuid() );
				}
				else
				{
					IA = qobject_cast<fugio::AudioProducerInterface *>( IP->control()->qobject() );
				}

				mAudioOutputs.insert( i, AudioPinPair( IP, IA ) );
			}
		}
	}

	if( mPluginController )
	{
		for( int i = 0 ; i < mPluginController->getParameterCount() ; i++ )
		{
			ParameterInfo		PI;

			if( mPluginController->getParameterInfo( i, PI ) != kResultOk )
			{
				continue;
			}

			if( ( PI.flags & ParameterInfo::kCanAutomate ) == 0 )
			{
				continue;
			}

			QString		PinNam = QString( (const QChar *)&PI.title );

			QSharedPointer<fugio::PinInterface>	 IP = mNode->findInputPinByName( PinNam );

			if( !IP )
			{
				IP = pinInput( PinNam, QUuid::createUuid() );

				ParamValue V = mPluginController->getParamNormalized( PI.id  );

				IP->setValue( V );
			}

			ParamEntry			PE;

			PE.mIndex = i;
			PE.mQueue.setPin( IP );
			PE.mQueue.setParamID( PI.id );

			mParamList.append( PE );
		}
	}

	if( mPluginController )
	{
		Vst::IConnectionPoint* iConnectionPointComponent = 0;
		Vst::IConnectionPoint* iConnectionPointController = 0;

		mPluginComponent->queryInterface (Vst::IConnectionPoint::iid, (void**)&iConnectionPointComponent);
		mPluginController->queryInterface (Vst::IConnectionPoint::iid, (void**)&iConnectionPointController);

		try
		{
			if (iConnectionPointComponent && iConnectionPointController)
			{
				iConnectionPointComponent->connect (iConnectionPointController);
				iConnectionPointController->connect (iConnectionPointComponent);
			}
		}
		catch( ... )
		{

		}

		// synchronize controller to component by using setComponentState
		MemoryStream stream;
		//stream.setByteOrder (kLittleEndian);
		if (mPluginComponent->getState (&stream))
		{
			stream.seek( 0, kSeekSet, 0 );

			mPluginController->setComponentState (&stream);
		}
	}

	if( mPluginController )
	{
		for( ParamEntry &PE : mParamList )
		{
			ParameterInfo		PI;

			if( mPluginController->getParameterInfo( PE.mIndex, PI ) != kResultOk )
			{
				continue;
			}

			ParamValue V = mPluginController->getParamNormalized( PI.id  );

			PE.mQueue.pin()->setValue( QString::number( V ) );
		}
	}

	return( res );
}

bool VST3Node::deinitialise()
{
	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

	if( mPluginView )
	{
		delete mPluginView;

		mPluginView = 0;
	}

	if( mDockWidget )
	{
		EI->mainWindow()->removeDockWidget( mDockWidget );

		delete mDockWidget;

		mDockWidget = 0;
	}

	mInstanceMutex.lock();

	for( AudioInstanceData *AID : mInstanceList )
	{
		AID->close();
	}

	mInstanceMutex.unlock();

	bool controllerIsComponent = false;

	if( mPluginComponent )
	{
		mPluginComponent->setActive( false );

		controllerIsComponent = FUnknownPtr<Vst::IEditController>( mPluginComponent ).getInterface () != 0;

		mPluginComponent->terminate();
	}

	if( mPluginController && !controllerIsComponent )
	{
		mPluginController->terminate();
	}

	if( mPluginComponent )
	{
		mPluginComponent->release();

		mPluginComponent = 0;
	}

	if (mPluginController)
	{
		mPluginController->release();

		mPluginController = 0;
	}

	if( mPluginFactory )
	{
		mPluginFactory->release();

		mPluginFactory = 0;
	}

	if( mPluginLibrary.isLoaded() )
	{
#if defined( Q_OS_WIN )
		ExitModuleProc ExitModuleProcFunc = (ExitModuleProc)mPluginLibrary.resolve( kExitModuleProcName );

		if( ExitModuleProcFunc )
		{
			ExitModuleProcFunc();
		}
#endif

		mPluginLibrary.unload();
	}
#endif

	return( true );
}

#define MIDI_CLOCK      0xf8
#define MIDI_ACTIVE     0xfe
#define MIDI_STATUS_MASK 0x80
#define MIDI_SYSEX      0xf0
#define MIDI_EOX        0xf7
#define MIDI_START      0xFA
#define MIDI_STOP       0xFC
#define MIDI_CONTINUE   0xFB
#define MIDI_F9         0xF9
#define MIDI_FD         0xFD
#define MIDI_RESET      0xFF
#define MIDI_NOTE_ON    0x90
#define MIDI_NOTE_OFF   0x80
#define MIDI_CHANNEL_AT 0xD0
#define MIDI_POLY_AT    0xA0
#define MIDI_PROGRAM    0xC0
#define MIDI_CONTROL    0xB0
#define MIDI_PITCHBEND  0xE0
#define MIDI_MTC        0xF1
#define MIDI_SONGPOS    0xF2
#define MIDI_SONGSEL    0xF3
#define MIDI_TUNE       0xF6

void VST3Node::inputsUpdated( qint64 pTimeStamp )
{
	QList<Steinberg::Vst::Event>		VstEvt;

	for( auto it = mEventInputs.begin() ; it != mEventInputs.end() ; it++ )
	{
		if( !it.value()->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		fugio::MidiInterface	*IM = input<fugio::MidiInterface *>( it.value() );

		if( !IM )
		{
			continue;
		}

		Steinberg::Vst::Event		VE;

		VE.busIndex     = it.key();
		VE.sampleOffset = 0;
		VE.ppqPosition  = 0.0;
		VE.flags        = Steinberg::Vst::Event::kIsLive;

		for( fugio::MidiEvent &PE : IM->messages() )
		{
			quint8		Status = Pm_MessageStatus( PE.message ) & 0xF0;

			if( Status >= 0xF0 )
			{
				continue;
			}

			if( Status == MIDI_NOTE_OFF || ( Status == MIDI_NOTE_ON && Pm_MessageData2( PE.message ) == 0 ) )
			{
				VE.type = Steinberg::Vst::Event::kNoteOffEvent;

				VE.noteOff.channel  = Pm_MessageStatus( PE.message ) & 0x0F;
				VE.noteOff.noteId   = -1;
				VE.noteOff.pitch    = Pm_MessageData1( PE.message );
				VE.noteOff.tuning   = 0.0f;
				VE.noteOff.velocity = float( Pm_MessageData2( PE.message ) ) / 127.0f;

				VstEvt << VE;
			}
			else if( Status == MIDI_NOTE_ON )
			{
				VE.type = Steinberg::Vst::Event::kNoteOnEvent;

				VE.noteOn.channel  = Pm_MessageStatus( PE.message ) & 0x0F;
				VE.noteOn.length   = 0;
				VE.noteOn.noteId   = -1;
				VE.noteOn.pitch    = Pm_MessageData1( PE.message );
				VE.noteOn.tuning   = 0.0f;
				VE.noteOn.velocity = float( Pm_MessageData2( PE.message ) ) / 127.0f;

				VstEvt << VE;
			}
		}
	}

	if( VstEvt.isEmpty() )
	{
		return;
	}

	mInstanceMutex.lock();

	for( AudioInstanceData *AID : mInstanceList )
	{
		QMutexLocker	L( &AID->mEventMutex );

		AID->mEventList.append( VstEvt );
	}

	mInstanceMutex.unlock();
}

void VST3Node::audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const
{
	AudioInstanceData		*InsDat = static_cast<AudioInstanceData *>( pInstanceData );

	if( !InsDat )
	{
		return;
	}

	Q_UNUSED( pSamplePosition )

	QVector<float *>	DatInp;
	QVector<float *>	DatOut;

	DatInp.resize( pChannelCount );
	DatOut.resize( pChannelCount );

	//-------------------------------------------------------------------------

	if( InsDat->mDatInp.size() < pChannelCount )
	{
		InsDat->mDatInp.resize( pChannelCount );
	}

	for( int i = 0 ; i < pChannelCount ; i++ )
	{
		QVector<float>		&DatBuf = InsDat->mDatInp[ i ];

		if( DatBuf.size() < pSampleCount )
		{
			DatBuf.resize( pSampleCount );
		}

		memset( DatBuf.data(), 0, sizeof( float ) * pSampleCount );

		DatInp[ i ] = DatBuf.data();
	}

	//-------------------------------------------------------------------------

	if( InsDat->mDatOut.size() < pChannelCount )
	{
		InsDat->mDatOut.resize( pChannelCount );
	}

	for( int i = 0 ; i < pChannelCount ; i++ )
	{
		QVector<float>		&DatBuf = InsDat->mDatOut[ i ];

		if( DatBuf.size() < pSampleCount )
		{
			DatBuf.resize( pSampleCount );
		}

		DatOut[ i ] = DatBuf.data();
	}

	//-------------------------------------------------------------------------

	if( InsDat->mInstance )
	{
		InsDat->mInstance->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, (void **)DatInp.data() );
	}

	//-------------------------------------------------------------------------

	Vst::AudioBusBuffers	 AudioInputs;

	AudioInputs.channelBuffers32 = DatInp.data();
	AudioInputs.numChannels      = DatInp.size();
	AudioInputs.silenceFlags     = 0;

	//-------------------------------------------------------------------------

	Vst::AudioBusBuffers	 AudioOutputs;

	AudioOutputs.channelBuffers32 = DatOut.data();
	AudioOutputs.numChannels      = DatOut.size();
	AudioOutputs.silenceFlags     = 0;

	//-------------------------------------------------------------------------

	InsDat->mEventMutex.lock();

	Vst::ProcessData		 PD;

	PD.processMode           = Vst::kRealtime;
	PD.symbolicSampleSize    = Vst::kSample32;
	PD.numSamples            = pSampleCount;
	PD.numInputs             = 1;
	PD.numOutputs            = 1;
	PD.inputs                = &AudioInputs;
	PD.outputs               = &AudioOutputs;
	PD.inputEvents			 = InsDat;
	PD.inputParameterChanges = (Steinberg::Vst::IParameterChanges *)( this );

	if( InsDat->mAudioProcessor )
	{
		InsDat->mAudioProcessor->process( PD );
	}

	InsDat->mEventList.clear();

	InsDat->mEventMutex.unlock();

	//-------------------------------------------------------------------------

	for( int c = 0 ; c < pChannelCount ; c++ )
	{
		float		*SrcPtr = DatOut[ c ];
		float		*DstPtr = (float *)pBuffers[ c ];

		for( int i = 0 ; i < pSampleCount ; i++ )
		{
			DstPtr[ i ] += SrcPtr[ i ];
		}
	}
}

void VST3Node::audioLinked( QSharedPointer<fugio::PinInterface> pPin )
{
	if( !pPin->hasControl() )
	{
		return;
	}

	fugio::AudioProducerInterface *IAP = qobject_cast<fugio::AudioProducerInterface *>( pPin->control()->qobject() );

	if( !IAP )
	{
		return;
	}

	QMutexLocker	MutLck( &mInstanceMutex );

	for( AudioInstanceData *AID : mInstanceList )
	{
		AID->mInstance = IAP->audioAllocInstance( AID->sampleRate(), AID->sampleFormat(), AID->channels() );
	}
}

void VST3Node::audioUnlinked( QSharedPointer<fugio::PinInterface> pPin )
{
	if( !pPin->hasControl() )
	{
		return;
	}

	fugio::AudioProducerInterface *IAP = qobject_cast<fugio::AudioProducerInterface *>( pPin->control()->qobject() );

	if( !IAP )
	{
		return;
	}

	QMutexLocker	MutLck( &mInstanceMutex );

	for( AudioInstanceData *AID : mInstanceList )
	{
		if( AID->mInstance )
		{
			delete AID->mInstance;

			AID->mInstance = nullptr;
		}
	}
}

int32 VST3Node::getParameterCount( void )
{
	return( mParamList.size() );
}

Vst::IParamValueQueue *VST3Node::getParameterData( int32 index )
{
	for( ParamEntry &PE : mParamList )
	{
		if( PE.mIndex == index )
		{
			return( &PE.mQueue );
		}
	}

	return( nullptr );
}

Vst::IParamValueQueue *VST3Node::addParameterData( const Vst::ParamID &id, int32 &index )
{
	return( nullptr );
}

tresult VST3Node::beginEdit(ParamID id)
{
	return( kResultOk );
}

tresult VST3Node::performEdit(ParamID id, ParamValue valueNormalized)
{
	return( kResultOk );
}

tresult VST3Node::endEdit(ParamID id)
{
	return( kResultOk );
}

tresult VST3Node::restartComponent(int32 flags)
{
	return( kResultOk );
}

bool VST3Node::filterClassCategory (char *category, char *classCategory) const
{
	return strcmp (category, classCategory) == 0;
}


fugio::AudioInstanceBase *VST3Node::audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
{
	AudioInstanceData		*InsDat = new AudioInstanceData( qSharedPointerDynamicCast<fugio::AudioProducerInterface>( mNode->control() ), pSampleRate, pSampleFormat, pChannels );

	if( InsDat )
	{
		InsDat->mInstance = nullptr;

		InsDat->mAudioProcessor = nullptr;

		if( !mAudioInputs.isEmpty() )
		{
			if( fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( mAudioInputs.first() ) )
			{
				InsDat->mInstance = IAP->audioAllocInstance( pSampleRate, pSampleFormat, pChannels );
			}
		}

		if( mPluginComponent && mPluginComponent->queryInterface( Vst::IAudioProcessor::iid, (void**)&InsDat->mAudioProcessor) == kResultOk )
		{
			Vst::ProcessSetup		PS;

			PS.maxSamplesPerBlock = 1024;
			PS.processMode        = Vst::kRealtime;
			PS.sampleRate         = pSampleRate;
			PS.symbolicSampleSize = Vst::kSample32;

			if( InsDat->mAudioProcessor->setupProcessing( PS ) == kResultOk )
			{
				mPluginComponent->setActive( true );

				InsDat->mAudioProcessor->setProcessing( true );
			}
		}

		mInstanceMutex.lock();

		mInstanceList << InsDat;

		mInstanceMutex.unlock();
	}

	return( InsDat );
}

IMPLEMENT_FUNKNOWN_METHODS( VST3Node::AudioInstanceData, Steinberg::Vst::IEventList, Steinberg::Vst::IEventList::iid )

int32 VST3Node::AudioInstanceData::getEventCount()
{
	return( mEventList.size() );
}

tresult VST3Node::AudioInstanceData::getEvent( int32 index, Event &e )
{
	e = mEventList.at( index );

	return( kResultOk );
}

tresult VST3Node::AudioInstanceData::addEvent( Event &e )
{
	Q_UNUSED( e )

	return( kResultOk );
}

int VST3Node::audioChannels() const
{
	return( 0 );
}

qreal VST3Node::audioSampleRate() const
{
	return( 0 );
}

fugio::AudioSampleFormat VST3Node::audioSampleFormat() const
{
	return( fugio::AudioSampleFormat::FormatUnknown );
}

qint64 VST3Node::audioLatency() const
{
	qint64		MaxLat = 0;

	for( QSharedPointer<fugio::PinInterface> P : mAudioInputs.values() )
	{
		fugio::AudioProducerInterface *IAP = input<fugio::AudioProducerInterface *>( P );

		MaxLat = qMax( MaxLat, IAP ? IAP->audioLatency() : 0 );
	}

	return( MaxLat );
}


tresult VST3Node::setDirty(TBool state)
{
	return( kResultOk );
}

tresult VST3Node::requestOpenEditor(FIDString name)
{
	return( kResultOk );
}

tresult VST3Node::startGroupEdit()
{
	return( kResultOk );
}

tresult VST3Node::finishGroupEdit()
{
	return( kResultOk );
}

IContextMenu *VST3Node::createContextMenu(IPlugView *plugView, const ParamID *paramID)
{
	return( 0 );
}

#endif
