#include "vst2node.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

#include "dirlocker.h"
#include "vstplugin.h"

#if defined( VST_SUPPORTED )

#define VERSION_MAJOR		(1)
#define VERSION_MINOR		(0)
#define VERSION_PATCH		(1)

VstInt32			VST2Node::currentPluginUniqueId = 0;

#define PLUGIN_VST2X_ID_UNKNOWN "????"

typedef struct {
	unsigned long id;
	QByteArray idString;
} PluginVst2xIdMembers;

typedef PluginVst2xIdMembers *PluginVst2xId;

static QByteArray _convertIntIdToString( const unsigned long id )
{
	QByteArray	result = QByteArray( 4, '?' );

	int i;

	for (i = 0; i < 4; i++) {
		result[ i ] = (char)(id >> ((3 - i) * 8) & 0xff);
	}

	return result;
}

static unsigned long _convertStringIdToInt(const QByteArray idString)
{
	unsigned long result = 0;
	int i;

	if( idString.size() == 4 )
	{
		for (i = 0; i < 4; i++) {
			result |= (unsigned long)(idString[i]) << ((3 - i) * 8);
		}
	}

	return result;
}

PluginVst2xId newPluginVst2xId(void)
{
	PluginVst2xId pluginVst2xId = new PluginVst2xIdMembers;

	pluginVst2xId->id = 0;
	pluginVst2xId->idString = QByteArray( PLUGIN_VST2X_ID_UNKNOWN );

	return pluginVst2xId;
}

PluginVst2xId newPluginVst2xIdWithId(unsigned long id)
{
	PluginVst2xId pluginVst2xId = newPluginVst2xId();

	pluginVst2xId->id = id;
	pluginVst2xId->idString = _convertIntIdToString(id);

	return pluginVst2xId;
}

PluginVst2xId newPluginVst2xIdWithStringId(const QByteArray idString)
{
	PluginVst2xId pluginVst2xId = newPluginVst2xId();

	pluginVst2xId->id = _convertStringIdToInt(idString);

	if( !idString.isEmpty() && pluginVst2xId->id > 0)
	{
		pluginVst2xId->idString = idString;
	}

	return pluginVst2xId;
}

void freePluginVst2xId(PluginVst2xId self)
{
	if (self)
	{
		delete self;
	}
}

int _canHostDo( const char *pluginName, const char *canDoString )
{
	bool supported = false;

	qDebug() << pluginName << canDoString;

	if (!strcmp(canDoString, "sendVstEvents"))
	{
		supported = true;
	}
	else if (!strcmp(canDoString, "sendVstMidiEvent"))
	{
		supported = true;
	}

	return( supported ? 1 : 0 );
}
#endif

VST2Node::VST2Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	//	pinOutput<InterfacePinControl *>( "Trigger", mPinOutput, PID_TRIGGER );
}

bool VST2Node::initialise()
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

	VSTPluginMain				PluginMain;

	if( ( PluginMain = (VSTPluginMain)mPluginLibrary.resolve( "VSTPluginMain" ) ) == nullptr )
	{
		return( false );
	}

	if( ( mEffect = PluginMain( &VST2Node::audioMaster ) ) == nullptr )
	{
		return( false );
	}

	if( mEffect->magic != kEffectMagic )
	{
		return( false );
	}

	mEffect->user = this;

	mDispatcher = mEffect->dispatcher;

	dispatch( effOpen );

	for( int i = 0 ; i < mEffect->numParams ; i++ )
	{
		char		PrmNam[ 32 ];

		dispatch( effGetParamName, i, 0, &PrmNam );

		QString		PinNam = QString( PrmNam );

		QSharedPointer<fugio::PinInterface>	 IP = mNode->findInputPinByName( PinNam );

		if( !IP )
		{
			IP = pinInput( PinNam, QUuid::createUuid() );

//			ParamValue V = mPluginController->getParamNormalized( PI.id  );

//			IP->setValue( V );
		}

	}
#endif

	return( false );
}

bool VST2Node::deinitialise()
{
	return( NodeControlBase::deinitialise() );
}

void VST2Node::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	//	mNode->context()->pinUpdated( mPinOutput );
}

#if defined( VST_SUPPORTED )
VstIntPtr VST2Node::audioMaster( AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt )
{
	PluginVst2xId pluginId;

	if( effect )
	{
		pluginId = newPluginVst2xIdWithId( (unsigned long)effect->uniqueID );
	}
	else
	{
		// During plugin initialization, the dispatcher can be called without a
		// valid plugin instance, as the AEffect* struct is still not fully constructed
		// at that point.
		pluginId = newPluginVst2xId();
	}

	const char *pluginIdString = pluginId->idString.data();

	VstIntPtr result = 0;

	switch( opcode )
	{
		case audioMasterAutomate:
			break;

		case audioMasterVersion:
			result = kVstVersion;
			break;

		case audioMasterCurrentId:
			result = currentPluginUniqueId;
			break;

		case audioMasterIdle:
			result = 1;
			break;

		case audioMasterGetTime:
			break;

		case audioMasterIOChanged:
			{
//				if (effect != NULL)
//				{
//					PluginChain pluginChain = getPluginChain();
//				}
			}
			break;

		case audioMasterGetSampleRate:
			result = 48000;
			break;

		case audioMasterGetVendorString:
			strncpy( (char *)dataPtr, "Fugio", kVstMaxVendorStrLen );
			result = 1;
			break;

		case audioMasterGetProductString:
			strncpy( (char *)dataPtr, "Fugio", kVstMaxProductStrLen );
			result = 1;
			break;

		case audioMasterGetVendorVersion:
			// Return our version as a single string, in the form ABCC, which corresponds to version A.B.C
			// Often times the patch can reach double-digits, so it gets two decimal places.
			result = VERSION_MAJOR * 1000 + VERSION_MINOR * 100 + VERSION_PATCH;
			break;

		case audioMasterGetLanguage:
			switch( QLocale().language() )
			{
				case QLocale::French:
					result = kVstLangFrench;
					break;

				case QLocale::German:
					result = kVstLangGerman;
					break;

				case QLocale::Italian:
					result = kVstLangItalian;
					break;

				case QLocale::Spanish:
					result = kVstLangSpanish;
					break;

				case QLocale::Japanese:
					result = kVstLangJapanese;
					break;

				default:
					result = kVstLangEnglish;
					break;
			}
			break;

		case audioMasterCanDo:
			result = _canHostDo( pluginIdString, (char *)dataPtr );
			break;

		default:
			qWarning() << QString( "Plugin '%1' asked if host can do unknown opcode %2" ).arg( pluginIdString ).arg( opcode );
			break;
	}

	freePluginVst2xId( pluginId );

	return result;
}

void VST2Node::dispatch( VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt )
{
	mDispatcher( mEffect, opcode, index, value, dataPtr, opt );
}
#endif
