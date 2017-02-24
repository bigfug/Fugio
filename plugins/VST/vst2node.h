#ifndef VST2NODE_H
#define VST2NODE_H

#include <QObject>
#include <QLibrary>

#include <fugio/nodecontrolbase.h>

#if defined( VST_SUPPORTED )

#include <base/source/fobject.h>
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <pluginterfaces/vst2.x/aeffect.h>
#include <pluginterfaces/vst2.x/aeffectx.h>

extern "C"
{
typedef AEffect *(PLUGIN_API *VSTPluginMain)( audioMasterCallback audioMaster );
}

#endif

class VST2Node : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE VST2Node( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~VST2Node( void ) {}

	// InterfaceNodeControl interface

	//-------------------------------------------------------------------------
	// InterfaceNodeControl

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	friend class VSTPlugin;

#if defined( VST_SUPPORTED )
	static VstIntPtr VSTCALLBACK audioMaster( AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* dataPtr, float opt );

	void dispatch( VstInt32 opcode, VstInt32 index = 0, VstIntPtr value = 0, void *dataPtr = 0, float opt = 0.0f );
#endif

protected:
#if defined( VST_SUPPORTED )
	static VstInt32							 currentPluginUniqueId;

	QLibrary								 mPluginLibrary;
	AEffect									*mEffect;

	AEffectDispatcherProc					 mDispatcher;
#endif

//	QSharedPointer<InterfacePin>			 mPinOutput;
};

#endif // VST2NODE_H
