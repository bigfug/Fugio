#ifndef VSTNODE_H
#define VSTNODE_H

// Don't add Q_DECL_OVERRIDE here as the VST includes use the sort
// of nested #defines that can't be easily changed, resulting in
// lots of compiler warnings

#include <QObject>
#include <QLibrary>
#include <QDockWidget>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_instance_base.h>

#include <fugio/core/uuid.h>
#include <fugio/vst/uuid.h>

#include <fugio/nodecontrolbase.h>

#if defined( VST_SUPPORTED )

#include <base/source/fobject.h>
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"

namespace Steinberg {
namespace Vst {

class IComponent;
class IEditController;
}}

//using namespace Steinberg;
//using namespace Steinberg::Vst;
#endif

class VSTView;

namespace Steinberg {
namespace Vst {

#if defined( VST_SUPPORTED )


class ParameterValueQueue : public IParamValueQueue
{
public:
//--------------------------------------------------------------------------------------------------------------
	ParameterValueQueue (ParamID paramID);
	virtual ~ParameterValueQueue ();

	ParamID PLUGIN_API getParameterId () { return paramID; }
	int32 PLUGIN_API getPointCount ();
	tresult PLUGIN_API getPoint (int32 index, int32& sampleOffset, ParamValue& value);
	tresult PLUGIN_API addPoint (int32 sampleOffset, ParamValue value, int32& index);

	void setParamID (ParamID pID) {paramID = pID;}
	void clear ();

	void setPin( QSharedPointer<fugio::PinInterface> pPin )
	{
		mPin = pPin;
	}

	QSharedPointer<fugio::PinInterface> pin( void )
	{
		return( mPin );
	}

//--------------------------------------------------------------------------------------------------------------
	DECLARE_FUNKNOWN_METHODS
protected:
	ParamID paramID;

	struct ParameterQueueValue
	{
		ParameterQueueValue (ParamValue value, int32 sampleOffset) : value (value), sampleOffset (sampleOffset) {}
		ParamValue value;
		int32 sampleOffset;
	};
	std::vector<ParameterQueueValue> values;

	QSharedPointer<fugio::PinInterface>			mPin;
};

#endif

}}

class VST3Node : public fugio::NodeControlBase, public fugio::AudioProducerInterface
#if defined( VST_SUPPORTED )
		, public Steinberg::FObject, public Steinberg::Vst::IParameterChanges, public Steinberg::Vst::IComponentHandler
#endif
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

public:
	Q_INVOKABLE explicit VST3Node( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~VST3Node( void );

	//-------------------------------------------------------------------------
	// InterfaceNodeControl

	virtual bool initialise( void );

	virtual bool deinitialise( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

	//-------------------------------------------------------------------------
	// fugio::AudioProducerInterface interface

public:
	virtual fugio::AudioInstanceBase *audioAllocInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels );
//	virtual void audioFreeInstance(void *pInstanceData);
	virtual int audioChannels() const;
	virtual qreal audioSampleRate() const;
	virtual fugio::AudioSampleFormat audioSampleFormat() const;
	virtual qint64 audioLatency() const;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const
	{
		Q_UNUSED( pInstance )

		return( true );
	}

#if defined( VST_SUPPORTED )
	//------------------------------------------------------------------------

	/** Returns count of Parameter changes in the list. */
	virtual Steinberg::int32 PLUGIN_API getParameterCount ();

	/** Returns the queue at a given index. */
	virtual Steinberg::Vst::IParamValueQueue* PLUGIN_API getParameterData (Steinberg::int32 index);

	/** Adds a new parameter queue with a given ID at the end of the list,
	returns it and its index in the parameter changes list. */
	virtual Steinberg::Vst::IParamValueQueue* PLUGIN_API addParameterData (const Steinberg::Vst::ParamID& id, Steinberg::int32& index /*out*/);

	//------------------------------------------------------------------------
	/** To be called before calling a performEdit (e.g. on mouse-click-down event). */
	virtual Steinberg::tresult PLUGIN_API beginEdit (Steinberg::Vst::ParamID id);

	/** Called between beginEdit and endEdit to inform the handler that a given parameter has a new value. */
	virtual Steinberg::tresult PLUGIN_API performEdit (Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized);

	/** To be called after calling a performEdit (e.g. on mouse-click-up event). */
	virtual Steinberg::tresult PLUGIN_API endEdit (Steinberg::Vst::ParamID id);

	/** Instructs host to restart the component. This should be called in the UI-Thread context!
		\param flags is a combination of RestartFlags */
	virtual Steinberg::tresult PLUGIN_API restartComponent (Steinberg::int32 flags);

	//------------------------------------------------------------------------

	OBJ_METHODS(VST3Node, FObject)
	REFCOUNT_METHODS(FObject)
	DEF_INTERFACES_2(IParameterChanges, IComponentHandler, FObject)

	//------------------------------------------------------------------------
#endif

protected:
#if defined( VST_SUPPORTED )
	bool filterClassCategory( char *category, char *classCategory) const;
#endif

private:
#if defined( VST_SUPPORTED )
	class AudioInstanceData : public fugio::AudioInstanceBase, public Steinberg::Vst::IEventList
	{
		DECLARE_FUNKNOWN_METHODS

	public:
		AudioInstanceData( QSharedPointer<fugio::AudioProducerInterface> pProducer, qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels )
			: fugio::AudioInstanceBase( pProducer, pSampleRate, pSampleFormat, pChannels )
		{
			FUNKNOWN_CTOR
		}

		virtual ~AudioInstanceData( void )
		{
			close();

			FUNKNOWN_DTOR
		}

		virtual void audio( qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers )
		{
			if( !mAudioProcessor )
			{
				return;
			}

			QSharedPointer<VST3Node>	API = qSharedPointerCast<VST3Node>( mProducer );

			if( !API )
			{
				return;
			}

			API->audio( pSamplePosition, pSampleCount, pChannelOffset, pChannelCount, pBuffers, this );
		}

		void close( void )
		{
			if( mAudioProcessor )
			{
				mAudioProcessor->setProcessing( false );

				mAudioProcessor->release();

				mAudioProcessor = 0;
			}
		}

		//------------------------------------------------------------------------
		// IEventList interface

		virtual Steinberg::int32 PLUGIN_API getEventCount();
		virtual Steinberg::tresult PLUGIN_API getEvent(Steinberg::int32 index, Steinberg::Vst::Event &e);
		virtual Steinberg::tresult PLUGIN_API addEvent(Steinberg::Vst::Event &e);

	public:
		fugio::AudioInstanceBase				*mInstance;
		Steinberg::Vst::IAudioProcessor			*mAudioProcessor;

		QList<Steinberg::Vst::Event>			 mEventList;

		QVector<QVector<float>>					 mDatInp;
		QVector<QVector<float>>					 mDatOut;
	};

//	class AudioInstanceData : public Steinberg::Vst::IEventList
//	{
//		DECLARE_FUNKNOWN_METHODS

//	public:
//		AudioInstanceData( void )
//		{
//			FUNKNOWN_CTOR
//		}

//		virtual ~AudioInstanceData( void )
//		{
//			FUNKNOWN_DTOR
//		}

//		//------------------------------------------------------------------------
//		// IEventList interface

//		virtual Steinberg::int32 PLUGIN_API getEventCount();
//		virtual Steinberg::tresult PLUGIN_API getEvent(Steinberg::int32 index, Steinberg::Vst::Event &e);
//		virtual Steinberg::tresult PLUGIN_API addEvent(Steinberg::Vst::Event &e);

//	public:
//		qreal									mSampleRate;
//		fugio::AudioSampleFormat						mSampleFormat;
//		int										mChannels;

//		void									*mInstance;
//		Steinberg::Vst::IAudioProcessor			*mAudioProcessor;

//		QMutex									 mEventMutex;
//		QList<Steinberg::Vst::Event>			 mEventList;

//		QVector<QVector<float>>			 mDatInp;
//		QVector<QVector<float>>			 mDatOut;
//	};

	void audio(qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, void **pBuffers, AudioInstanceData *pInstanceData ) const;
#endif

private slots:
	void audioLinked( QSharedPointer<fugio::PinInterface> pPin );
	void audioUnlinked( QSharedPointer<fugio::PinInterface> pPin );

private:
#if defined( VST_SUPPORTED )
	QLibrary								 mPluginLibrary;
	Steinberg::IPluginFactory				*mPluginFactory;

	Steinberg::FUnknown						*mPluginContext;

	Steinberg::Vst::IComponent				*mPluginComponent;
	Steinberg::Vst::IEditController			*mPluginController;

	VSTView									*mPluginView;

	QDockWidget								*mDockWidget;
	Qt::DockWidgetArea						 mDockArea;

	typedef QPair<QSharedPointer<fugio::PinInterface>,fugio::AudioProducerInterface *>		AudioPinPair;
	typedef QMap<int,AudioPinPair>															AudioPinMap;

	QMap<int,QSharedPointer<fugio::PinInterface>>	 mAudioInputs;
	AudioPinMap										 mAudioOutputs;

	QMap<int,QSharedPointer<fugio::PinInterface>>	 mEventInputs;

	typedef struct ParamEntry
	{
		Steinberg::Vst::ParameterValueQueue		mQueue;
		int										mIndex;

		ParamEntry( void )
			: mQueue( 0 )
		{

		}
	} ParamEntry;

	QVector<ParamEntry>						 mParamList;

	QMutex									 mInstanceMutex;
	QList<AudioInstanceData *>				 mInstanceList;
#endif
};

#endif // VSTNODE_H
